// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityTargetActor.h"
#include "AP_TargetActor_SphereAOE.generated.h"

/**
 * given a location and a sphere, query all target that hit the sphere
 */
UCLASS(Blueprintable, notplaceable)
class LINHIBLADE_API AAP_TargetActor_SphereAOE : public AGameplayAbilityTargetActor
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;
	virtual void StartTargeting(UGameplayAbility* Ability) override;
	virtual void ConfirmTargetingAndContinue() override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (ExposeOnSpawn = true), Category = AOE)
		float Radius;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (ExposeOnSpawn = true), Category = AOE)
		TEnumAsByte<ECollisionChannel> Channel;
protected:
	TArray<TWeakObjectPtr<AActor>> PerformTrace();
};
