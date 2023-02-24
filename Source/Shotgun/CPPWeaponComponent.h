// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CPPWeaponComponent.generated.h"


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class SHOTGUN_API UCPPWeaponComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    // Sets default values for this component's properties
    UCPPWeaponComponent();

    UFUNCTION(BlueprintCallable, Category = "Weapon")
    FVector test();

    UFUNCTION(BlueprintCallable, Category = "Weapon")
    FVector GetRandomSpreadVector(FVector AimVector, double MaxSpreadAngleDegrees);

    UFUNCTION(BlueprintCallable, Category = "Weapon")
    FVector GetPathedSpreadVector(FVector AimVector, double MaxSpreadAngleDegrees, int badAppleIndex);

    UFUNCTION(BlueprintCallable, Category = "Weapon")
    FVector GetImageSpreadVector(FVector AimVector, double MaxSpreadAngleDegrees, int badAppleIndex);

    UFUNCTION(BlueprintCallable, Category = "Weapon")
    void StartBadApple();

    UFUNCTION(BlueprintCallable, Category = "Weapon")
    int GetCurrentFrameIndex();

protected:
    // Called when the game starts
    virtual void BeginPlay() override;

public:
    // Called every frame
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


private:
    struct pt {
        uint16 x;
        uint16 y;
    };

    TArray<TArray<pt>> badApplePaths;
    TArray<TBitArray<FDefaultBitArrayAllocator>> badAppleImages;
    //TBitArray<FDefaultBitArrayAllocator> imagePoints;
    FDateTime start;
    uint16 imageWidth;
    uint16 imageHeight;
    uint32 counter = 60;
    uint16 pointMax;
};
