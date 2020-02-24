// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Components/ActorComponent.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"
#include "AP_AbilityBase.h"
#include "AbilityUser.generated.h"

UENUM(BlueprintType)
enum class EAbilityState : uint8
{
	Ready 		UMETA(DisplayName = "Ready"),
	Casting 	UMETA(DisplayName = "Casting"),
	OnCooldown	UMETA(DisplayName = "On Cooldown"),
	Invalid		UMETA(DisplayName = "Invalid"),
};
UENUM(BlueprintType)
enum class EAbilityBehavior : uint8
{
	Manual 		UMETA(DisplayName = "Ready"),
	AutoCast 	UMETA(DisplayName = "Auto Cast"),
	Disabled	UMETA(DisplayName = "Disabled")
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class LINHIBLADE_API UAbilityUser : public UActorComponent
{
	GENERATED_BODY()

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAbilityEventDelegate, int, AbilitySlot);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FEffectAppliedDelegate, UAbilitySystemComponent*, Source, const FGameplayEffectSpec&, Spec, FActiveGameplayEffectHandle, Handle);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FEffectRemovedDelegate, const FGameplayEffectRemovalInfo&, Info);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FGenericEventDelegate);

public:	
	// Sets default values for this component's properties
	UAbilityUser();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
protected:
	UPROPERTY(ReplicatedUsing = OnRep_AbilityStates)
		TArray <EAbilityState> AbilityStates;
	UPROPERTY(ReplicatedUsing = OnRep_AbilityBehaviors)
		TArray <EAbilityBehavior> AbilityBehaviors;
	UPROPERTY(ReplicatedUsing = OnRep_AbilityHandles)
		TArray<FGameplayAbilitySpecHandle> AbilityHandles;
	UPROPERTY(ReplicatedUsing = OnRep_AbilityLevels)
		TArray<int32> AbilityLevels;
	UPROPERTY()
		UAbilitySystemComponent* AbilitySystem;
public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
public:
	UPROPERTY(BlueprintAssignable)
		FGenericEventDelegate OnAbilityStateChanged;
	UPROPERTY(BlueprintAssignable)
		FGenericEventDelegate OnAbilitySlotChanged;
	UPROPERTY(BlueprintAssignable)
		FAbilityEventDelegate OnAbilityLevelChanged;
	UPROPERTY(BlueprintAssignable)
		FEffectAppliedDelegate OnEffectApplied;
	UPROPERTY(BlueprintAssignable)
		FEffectRemovedDelegate OnEffectRemoved;
public:
	UFUNCTION(BlueprintCallable, Category = "Abilities")
		void GiveAbility(TSubclassOf<UAP_AbilityBase> Ability);
	UFUNCTION(BlueprintCallable, Category = "Abilities")
		void RemoveAbility(TSubclassOf<UAP_AbilityBase> Ability);
	UFUNCTION(Category = "Abilities")
		void HandleEffectApplied(UAbilitySystemComponent* Source, const FGameplayEffectSpec& Spec, FActiveGameplayEffectHandle Handle);
	UFUNCTION(Category = "Abilities")
		void HandleEffectRemoved(const FGameplayEffectRemovalInfo& Info);
	UFUNCTION(Category = "Abilities")
		void HandleAbilityActivated(UGameplayAbility* Ability);
	UFUNCTION(Category = "Abilities")
		void HandleAbilityCommitted(UGameplayAbility* Ability);
	UFUNCTION(Category = "Abilities")
		void HandleAbilityEnded(UGameplayAbility* Ability);
	UFUNCTION(Category = "Abilities")
		void HandleAbilityOffCooldown(const FGameplayEffectRemovalInfo& InGameplayEffectRemovalInfo);
	UFUNCTION(BlueprintCallable, Category = "Abilities")
		int GetAbilityCount() const;
	UFUNCTION(BlueprintPure, Category = "Abilities")
		UAP_AbilityBase* GetAbility(int AbilitySlot) const;
	UFUNCTION(BlueprintPure, Category = "Abilities")
		bool CanActivateAbility(int AbilitySlot) const;
	UFUNCTION(BlueprintCallable, Category = "Abilities")
		bool LevelUpAbility(int AbilitySlot);
	UFUNCTION(BlueprintCallable, Category = "Abilities")
		int ResetAbilityLevel(int AbilitySlot);
	UFUNCTION(BlueprintPure, Category = "Abilities")
		float GetAbilityCooldown(int AbilitySlot);
	UFUNCTION(BlueprintPure, Category = "Abilities")
		float GetAbilityCooldownPercent(int AbilitySlot);
	UFUNCTION(BlueprintPure, Category = "Abilities")
		EAbilityState GetAbilityState(int AbilitySlot);
	UFUNCTION(BlueprintPure, Category = "Abilities")
		bool GetAbilityAutoCastEnabled(int AbilitySlot);
	UFUNCTION(BlueprintPure, Category = "Abilities")
		bool IsAbilityBusy(int AbilitySlot);
	UFUNCTION(BlueprintCallable, Category = "Abilities")
		void RemoveAllAbilities();
	UFUNCTION(BlueprintPure, Category = "Abilities")
		int GetAbilityLevel(int AbilitySlot);
	UFUNCTION(BlueprintCallable, Category = "Abilities")
		void ActivateAbility(int AbilitySlot);
	UFUNCTION(BlueprintCallable, Category = "Abilities")
		bool ActivateAbilityWithPayload(int AbilitySlot, FGameplayTag EventTag, FGameplayEventData Payload);
protected:
	UFUNCTION()
		virtual void OnRep_AbilityStates();
	void RecheckAbilityState();
	UFUNCTION()
		virtual void OnRep_AbilityBehaviors();
	UFUNCTION()
		virtual void OnRep_AbilityHandles();
	void RecheckAbilitySlot();
	UFUNCTION()
		virtual void OnRep_AbilityLevels();
	void RecheckAbilityLevel(int AbilitySlot = -1);
};
