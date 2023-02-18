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

FVector UCPPWeaponComponent::test()
{
    return FVector(FMath::RandRange(0.0, 1.0), FMath::RandRange(0.0, 1.0), FMath::RandRange(0.0, 1.0));
}

FVector UCPPWeaponComponent::GetRandomSpreadVector(FVector AimVector, double MaxSpreadAngleDegrees)
{
    // Assume AimVector is normalized

    // Find vector perpendicular to AimVector to use as rotation axis
    // To do this, we cross AimVector with the basis vector that has
    // the lowest magnitude in AimVector. This is because if we crossed
    // AimVector with itself, it would be zero

    // NOTE: IRL I'd simply use FindBestAxisVectors and pick one of those :P
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
    //// Sanitize Inputs
    //if (AimVector.Size() <= 0.f)
    //{
    //    AimVector = FVector::ForwardVector;
    //}
    //AimVector.Normalize();

    //// We're going to use the plane perpendicular to the aim direction.  So first calculate the vectors that represent
    //// the axis of the plane
    //FVector PlaneLeft = AimVector.Cross(FVector::UpVector);
    //if (PlaneLeft.Size() <= 0.f)
    //{
    //    // AimDir is straight up or down.  Choose arbitrary "left"
    //    PlaneLeft = -FVector::RightVector;
    //}

    //const FVector PlaneUp = PlaneLeft.Cross(AimVector);

    //// Next, calculate the center of the circle that will be on the plane
    //// Note: 100 is arbitrary, as long as we use the same value w/ the tangent function
    //const FVector CircleCenter = AimVector * 100.f;

    //// The radius of the circle, which we can calculate using the cone half-arc.
    //float CircleRadius = FMath::Tan(FMath::DegreesToRadians(MaxSpreadAngleDegrees)) * 100.f;

    //// Calculate a random point in this circle.  We'll do this in 2 parts
    //// 1. A random point along the radius, as the distance to the center of the circle
    //float pointR = FMath::Sqrt(FMath::FRand()) * CircleRadius;

    //// 2. A random angle around the circle
    //float angle = FMath::FRand() * TWO_PI;

    //// Now turn the polar coordinates into x,y coordinates
    //float pointX = pointR * FMath::Cos(angle);
    //float pointY = pointR * FMath::Sin(angle);

    //// Use the plane's X and Y vectors to generate offset vectors
    //FVector pointXOffset = PlaneLeft * pointX;
    //FVector pointYOffset = PlaneUp * pointY;
    //FVector pointOffset = pointXOffset + pointYOffset;

    //// Now add the circle center's offset to get the point we want the pellet to cross
    //FVector worldSpacePointOffset = pointOffset + CircleCenter;

    //worldSpacePointOffset.Normalize();

    //return worldSpacePointOffset;

    // NOTE: IRL I'd simply use FindBestAxisVectors and pick one of those :P
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

