// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "AP_Task_MoveNearUnit.generated.h"


/**
 * Move near a location, target is a AI controlled character
 * The task compeleted when target is reached or AI returned error
 */
UCLASS()
class LINHIBLADE_API UAP_Task_MoveNearUnit : public UAbilityTask
{
	GENERATED_BODY()

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FTargetReachedDelegate);
public:
	UPROPERTY(BlueprintAssignable)
	FTargetReachedDelegate OnTargetLocationReached;
	UPROPERTY(BlueprintAssignable)
	FTargetReachedDelegate OnMovementCancelled;
	TScriptDelegate<FWeakObjectPtr> OnMovementCompletedDelegate;
	TScriptDelegate<FWeakObjectPtr> OnMovementCancelledDelegate;
public:
	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
		static UAP_Task_MoveNearUnit* MoveNearUnit(UGameplayAbility* OwningAbility, FName TaskInstanceName, 
			AActor* Unit, float Range);
	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
		static UAP_Task_MoveNearUnit* MoveNearUnitTarget(UGameplayAbility* OwningAbility, FName TaskInstanceName, 
			FGameplayAbilityTargetDataHandle TargetHandle, float Range);
public:
	UAP_Task_MoveNearUnit();
	~UAP_Task_MoveNearUnit();
	virtual void Activate() override;
	virtual void OnDestroy(bool AbilityIsEnding) override;
public:
	UFUNCTION(BlueprintInternalUseOnly, Category = "Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
	void OnMovementCompleted(FAIRequestID RequestID, EPathFollowingResult::Type Result);
protected:
	UPROPERTY()
		AActor* TargetUnit;

	UPROPERTY()
		float AcceptanceRadius;

};
