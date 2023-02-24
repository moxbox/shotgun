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

    const int numBadAppleImages = 6562;
    int count = 0;
    FString projectDir = FPaths::ProjectDir();
    start = FDateTime::Now();

    // First load the edge paths
    for (int i = 1; i < numBadAppleImages; ++i) {
        FString path = FString::Printf(TEXT("%sContent/BadApple/Paths/bad_apple_%03d_Path.bin"), *projectDir, i);

        TArray<uint8> bytes;
        bool loaded = FFileHelper::LoadFileToArray(bytes, *path);

        if (loaded) {
            // populate class variable
            int size = bytes.Num() / 4; // each point is four bytes (2 each x,y)
            auto ptr = bytes.GetData();
            const pt* pPts = reinterpret_cast<pt*>(ptr);
            badApplePaths.Add(TArray<pt>(pPts, size));
        }
        else {
            UE_LOG(LogTemp, Display, TEXT("Path %d not loaded"), i);
        }
    }

    // Then load the binary images
    for (int i = 1; i < numBadAppleImages; ++i) {
        FString path = FString::Printf(TEXT("%sContent/BadApple/bad_apple_%03d.bin"), *projectDir, i);

        TArray<uint8> bytes;
        bool loaded = FFileHelper::LoadFileToArray(bytes, *path);

        if (loaded) {
            // populate class variable
            int numBits = bytes.Num() * 8; // each point is one bit
            imageHeight = 536;
            imageWidth = 720;
            if (numBits != imageHeight * imageWidth) {
                UE_LOG(LogTemp, Display, TEXT("Unexpected Input Size"));
            }
            auto* ptr = bytes.GetData();
            badAppleImages.Add(TBitArray<FDefaultBitArrayAllocator>(false, numBits));
            auto* pBitArray = badAppleImages.Last().GetData();
            if (badAppleImages.Last().GetAllocatedSize() == bytes.Num()) {
                FMemory::Memcpy(pBitArray, ptr, bytes.Num());
                count++;
            }
            else {
                UE_LOG(LogTemp, Display, TEXT("Image %d not loaded"), i);
            }
        }
        else {
            UE_LOG(LogTemp, Display, TEXT("Image %d not loaded"), i);
        }
    }

    UE_LOG(LogTemp, Display, TEXT("%d out of %d images loaded"), count, numBadAppleImages);
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

FVector UCPPWeaponComponent::GetPathedSpreadVector(FVector AimVector, double MaxSpreadAngleDegrees, int badAppleIndex)
{
    // If we don't have any path to use, return
    //uint32_t badAppleIndex = GetCurrentFrameIndex();
    if (!badApplePaths.IsValidIndex(badAppleIndex) || badApplePaths[badAppleIndex].IsEmpty()) {
        return AimVector;
    }

    // Pick a random point in our path
    int index = FMath::RandRange(0, badApplePaths[badAppleIndex].Num() - 1);
    auto pt = badApplePaths[badAppleIndex][index];

    // Convert index to x,y (image starts bl, need to convert so coords are relative to center)
    double x = (imageWidth / 2.0) - pt.x - 4.0;
    double y = pt.y + 4.0 - (imageHeight / 2.0);

    // Calculate the spread angle 
    double maxSpreadAngleRads = FMath::DegreesToRadians(MaxSpreadAngleDegrees);

    double maxCornerDistInPlane = FMath::Sqrt(pow(imageWidth / 2.0, 2.0) + pow(imageHeight / 2.0, 2.0));

    double spreadAngleX = -1.0 * FMath::Atan(x * FMath::Tan(maxSpreadAngleRads) / maxCornerDistInPlane);
    double spreadAngleY = -1.0 * FMath::Atan(y * FMath::Tan(maxSpreadAngleRads) / maxCornerDistInPlane);

    // Rotate away from AimVector by "random" spread angles
    FVector ortho1, ortho2;
    AimVector.FindBestAxisVectors(ortho1, ortho2);

    FVector adjustedVector = AimVector.RotateAngleAxisRad(spreadAngleX, ortho1);
    adjustedVector = adjustedVector.RotateAngleAxisRad(spreadAngleY, ortho2);
    adjustedVector.Normalize();

    return adjustedVector;

}

FVector UCPPWeaponComponent::GetImageSpreadVector(FVector AimVector, double MaxSpreadAngleDegrees, int badAppleIndex)
{
    // pick a random point in our image
    const int maxTries = 1000;
    
    //uint32_t badAppleIndex = GetCurrentFrameIndex();
    if (!badAppleImages.IsValidIndex(badAppleIndex)) {
        return AimVector;
    }

    int index = -1;
    bool found = false;
    for (int i = 0; i < maxTries; ++i) {
        index = FMath::RandRange(0, (imageWidth * imageHeight) - 1);

        // Check if found index is dark (binary 0)
        if (badAppleImages[badAppleIndex].IsValidIndex(index) && !badAppleImages[badAppleIndex][index]) {
            found = true;
            break;
        }
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
    FVector ortho1, ortho2;
    AimVector.FindBestAxisVectors(ortho1, ortho2);

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

int UCPPWeaponComponent::GetCurrentFrameIndex()
{
    auto elapsed = FDateTime::Now() - start;
    const double frameTimeMs = 1000.0 / 30.0;
    double framesElapsed = elapsed.GetTotalMilliseconds() / frameTimeMs;
    return FMath::RoundToInt32(framesElapsed) % (badAppleImages.Num() - 1);
}

void UCPPWeaponComponent::StartBadApple()
{
    start = FDateTime::Now();
}

