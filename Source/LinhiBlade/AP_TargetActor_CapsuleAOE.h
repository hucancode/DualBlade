// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityTargetActor.h"
#include "AP_TargetActor_CapsuleAOE.generated.h"

/**
 * given a location and a capsule, query all target that hit the capsule
 */
UCLASS()
class LINHIBLADE_API AAP_TargetActor_CapsuleAOE : public AGameplayAbilityTargetActor
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;
	virtual void StartTargeting(UGameplayAbility* Ability) override;
	virtual bool ShouldProduceTargetData() const override;

	/** Radius of target acquisition around the ability's start location. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (ExposeOnSpawn = true), Category = AOE)
		FVector Location;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (ExposeOnSpawn = true), Category = AOE)
		float Radius;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (ExposeOnSpawn = true), Category = AOE)
		TEnumAsByte<ECollisionChannel> Channel;
protected:
	TArray<TWeakObjectPtr<AActor>> PerformTrace();
};
