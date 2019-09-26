// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "AP_Task_LookAtUnit.generated.h"


/**
 * Look at a location, target is a AI controlled character
 * The task compeleted when target is reached or character is not idle
 */
UCLASS()
class LINHIBLADE_API UAP_Task_LookAtUnit : public UAbilityTask
{
	GENERATED_BODY()
		
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FRotationReachedDelegate);
public:
	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
		static UAP_Task_LookAtUnit* LookAtUnit(UGameplayAbility* OwningAbility, FName TaskInstanceName, 
			AActor* Unit);
	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
		static UAP_Task_LookAtUnit* LookAtUnitTarget(UGameplayAbility* OwningAbility, FName TaskInstanceName, 
			FGameplayAbilityTargetDataHandle TargetHandle);
public:
	UPROPERTY(BlueprintAssignable)
		FRotationReachedDelegate OnTargetRotationReached;
	UPROPERTY(BlueprintAssignable)
		FRotationReachedDelegate OnRotationCancelled;
public:
	UAP_Task_LookAtUnit();
	~UAP_Task_LookAtUnit();
	virtual void Activate() override;
	virtual void OnDestroy(bool AbilityIsEnding) override;
	virtual void TickTask(float DeltaTime) override;
protected:
	UPROPERTY()
		AActor* TargetUnit;
	UPROPERTY()
		float TurnRate;
private:
	FRotator Destination;
	bool Reached;
};
