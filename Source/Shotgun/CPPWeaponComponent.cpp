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

FVector UCPPWeaponComponent::GetRandomSpreadVector(FVector AimVector)
{
    return FVector();
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

