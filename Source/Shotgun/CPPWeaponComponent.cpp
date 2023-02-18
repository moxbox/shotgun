// Fill out your copyright notice in the Description page of Project Settings.
#include "CPPWeaponComponent.h"

#include "Misc/FileHelper.h"
#include "Misc/Paths.h"

// Sets default values for this component's properties
UCPPWeaponComponent::UCPPWeaponComponent()
{
    // Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
    // off to improve performance if you don't need them.
    PrimaryComponentTick.bCanEverTick = true;
    {
        TArray<uint8> bytes;
        auto path = FPaths::ProjectDir() + "Content/pathPoints.bin";
        bool loaded = FFileHelper::LoadFileToArray(bytes, *path);

        if (loaded) {
            // populate class variable
            int size = bytes.Num() / 4; // each point is four bytes (2 each x,y)
            auto ptr = bytes.GetData();
            const pt* pPts = reinterpret_cast<pt*>(ptr);
            pathPoints = TArray<pt>(pPts, size);
        }
    }
    {
        TArray<uint8> bytes;
        auto path = FPaths::ProjectDir() + "Content/image.bin";
        bool loaded = FFileHelper::LoadFileToArray(bytes, *path);

        if (loaded) {
            // populate class variable
            int numBits = bytes.Num() * 8; // each point is one bit
            imageHeight = 540;
            imageWidth = 720;
            if (numBits != imageHeight * imageWidth) {
                UE_LOG(LogTemp, Display, TEXT("Unexpected Input Size"));
            }
            auto* ptr = bytes.GetData();
            imagePoints = TBitArray<FDefaultBitArrayAllocator>(false, numBits);
            auto* pBitArray = imagePoints.GetData();
            if (imagePoints.GetAllocatedSize() == bytes.Num()) {
                FMemory::Memcpy(pBitArray, ptr, bytes.Num());
                UE_LOG(LogTemp, Display, TEXT("Image Loaded"));
            }
            else {
                UE_LOG(LogTemp, Display, TEXT("Image NOT Loaded"));
            }
        }
    }
}

FVector UCPPWeaponComponent::test()
{
    return FVector(FMath::RandRange(0.0, 1.0), FMath::RandRange(0.0, 1.0), FMath::RandRange(0.0, 1.0));
}

FVector UCPPWeaponComponent::GetRandomSpreadVector(FVector AimVector, double MaxSpreadAngleDegrees)
{
    // Assume AimVector is normalized
    // Find vector perpendicular to AimVector to use as rotation axis
    FVector orthogonal, ortho2;
    AimVector.FindBestAxisVectors(orthogonal, ortho2);

    // Calculate a random spread angle 
    double maxSpreadAngleRads = FMath::DegreesToRadians(MaxSpreadAngleDegrees);
    double spreadAngle = FMath::Atan(FMath::Sqrt(FMath::RandRange(0.0, 1.0)) * FMath::Tan(maxSpreadAngleRads));

    // Rotate away from AimVector by random spread angle
    FVector adjustedVector = AimVector.RotateAngleAxisRad(spreadAngle, orthogonal);

    // Rotate our new vector around AimVector by a random value
    double rotateAngle = FMath::RandRange(-1.0 * PI, 1.0 * PI);
    FVector outVector = adjustedVector.RotateAngleAxisRad(rotateAngle, AimVector);
    outVector.Normalize();

    return outVector;
}

FVector UCPPWeaponComponent::GetPathedSpreadVector(FVector AimVector, double MaxSpreadAngleDegrees)
{
    FVector ortho1, ortho2;
    AimVector.FindBestAxisVectors(ortho1, ortho2);

    // pick a random point in our path
    int index = FMath::RandRange(0, pathPoints.Num() - 1);
    auto pt = pathPoints[index];

    // Normalize it
    const double maxY = 14336.0;
    double x = (static_cast<double>(pt.x) / maxY) - 0.5;
    double y = (static_cast<double>(pt.y) / maxY) - 0.5;

    // Calculate a random spread angle 
    double maxSpreadAngleRads = FMath::DegreesToRadians(MaxSpreadAngleDegrees);
    double spreadAngleX = -1.0 * FMath::Atan(x * FMath::Tan(maxSpreadAngleRads));
    double spreadAngleY = -1.0 * FMath::Atan(y * FMath::Tan(maxSpreadAngleRads));

    // Rotate away from AimVector by "random" spread angles
    FVector adjustedVector = AimVector.RotateAngleAxisRad(spreadAngleX, ortho1);
    adjustedVector = adjustedVector.RotateAngleAxisRad(spreadAngleY, ortho2);
    adjustedVector.Normalize();

    return adjustedVector;
}

FVector UCPPWeaponComponent::GetImageSpreadVector(FVector AimVector, double MaxSpreadAngleDegrees)
{
    FVector ortho1, ortho2;
    AimVector.FindBestAxisVectors(ortho1, ortho2);

    // pick a random point in our image
    const int maxTries = 1000;
    int index = -1;
    bool found = false;
    for (int i = 0; i < maxTries; ++i) {
        index = FMath::RandRange(0, (imageWidth * imageHeight) - 1);

        // Check if found index is dark (binary 0)
        if (!imagePoints[index]) {
            found = true;
            break;
        }

        UE_LOG(LogTemp, Display, TEXT("Bright Area, trying again"));
    }
    if (!found) {
        return AimVector;
    }

    // Convert index to x,y (image starts tl, need to convert so coords are relative to center)
    double x = (imageWidth / 2.0) - (index % imageWidth);
    double y = (imageHeight / 2.0) - (index / imageWidth);
    
    // Calculate the spread angle 
    double maxSpreadAngleRads = FMath::DegreesToRadians(MaxSpreadAngleDegrees);

    double maxCornerDistInPlane = FMath::Sqrt(pow(imageWidth / 2.0, 2.0) + pow(imageHeight / 2.0, 2.0));

    double spreadAngleX = -1.0 * FMath::Atan(x * FMath::Tan(maxSpreadAngleRads) / maxCornerDistInPlane);
    double spreadAngleY = -1.0 * FMath::Atan(y * FMath::Tan(maxSpreadAngleRads) / maxCornerDistInPlane);

    // Rotate away from AimVector by "random" spread angles
    FVector adjustedVector = AimVector.RotateAngleAxisRad(spreadAngleX, ortho1);
    adjustedVector = adjustedVector.RotateAngleAxisRad(spreadAngleY, ortho2);
    adjustedVector.Normalize();

    return adjustedVector;
}


// Called when the game starts
void UCPPWeaponComponent::BeginPlay()
{
    Super::BeginPlay();

    // ...

}


// Called every frame
void UCPPWeaponComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    // ...
}

