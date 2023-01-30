// Fill out your copyright notice in the Description page of Project Settings.


#include "CPPWeaponComponent.h"

// Sets default values for this component's properties
UCPPWeaponComponent::UCPPWeaponComponent()
{
    // Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
    // off to improve performance if you don't need them.
    PrimaryComponentTick.bCanEverTick = true;

    // ...
}

FVector UCPPWeaponComponent::test()
{
    return FVector(FMath::RandRange(0.0, 1.0), FMath::RandRange(0.0, 1.0), FMath::RandRange(0.0, 1.0));
}

FVector UCPPWeaponComponent::GetRandomSpreadVector(FVector AimVector, double MaxSpreadAngleDegrees)
{
    // Assume AimVector is normalized
    AimVector.Normalize();

    // Find vector perpendicular to AimVector to use as rotation axis
    // To do this, we cross AimVector with the basis vector that has
    // the lowest magnitude in AimVector. This is because if we crossed
    // AimVector with itself, it would be zero

    // NOTE: IRL I'd simply use FindBestAxisVectors and pick one of those :P
    FVector orthogonal;
    int minIndex = 0;
    double curMin = DOUBLE_BIG_NUMBER;
    for (int i = 0; i < 3; ++i) {
        if (FMath::Abs(AimVector[i]) < curMin) {
            minIndex = i;
            curMin = AimVector[i];
        }
    }

    if (minIndex == 0) {
        orthogonal = AimVector.Cross(FVector::UnitX());
    }
    else if (minIndex == 1) {
        orthogonal = AimVector.Cross(FVector::UnitY());
    }
    else if (minIndex == 2) {
        orthogonal = AimVector.Cross(FVector::UnitZ());
    }

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

