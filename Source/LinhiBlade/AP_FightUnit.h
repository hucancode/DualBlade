// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameplayTagContainer.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"
#include "GenericTeamAgentInterface.h"
#include "TeamAgent.h"
#include "ItemUser.h"
#include "AbilityUser.h"
#include "AP_AttributeSet.h"
#include "AP_FightUnit.generated.h"

#ifndef UE_LOG_FAST
#define UE_LOG_FAST(Format, ...) UE_LOG(LogTemp, Display, Format, ##__VA_ARGS__)
#endif

#define EXP_AQUIRING_RANGE 800.0f

UCLASS()
class LINHIBLADE_API AAP_FightUnit : public ACharacter, public IAbilitySystemInterface, public IGameplayTagAssetInterface, public IGenericTeamAgentInterface
{
	GENERATED_BODY()

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDeathEventDelegate, const float, DeathTime);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FRespawnEventDelegate);
public:
	// Sets default values for this character's properties
	AAP_FightUnit();

protected:
	// Called when the game starts or when spawned
	virtual void				BeginPlay() override;

public:	
	virtual void				Tick(float DeltaTime) override;
	virtual void				SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void				PossessedBy(AController* NewController);
	virtual void				OnRep_Controller() override;
	virtual void				GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	UAbilitySystemComponent*	GetAbilitySystemComponent() const override;
	virtual void				SetGenericTeamId(const FGenericTeamId& Id) override;
	virtual FGenericTeamId		GetGenericTeamId() const override;
	virtual void				GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const override;
	virtual bool				HasMatchingGameplayTag(FGameplayTag TagToCheck) const override;
	virtual bool				HasAllMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const override;
	virtual bool				HasAnyMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const override;
public:
	void						HandleAbilityStateChanged(int AbilitySlot);
	void						HandleAbilitySlotChanged(int AbilitySlot);
	void						HandleEffectApplied(UAbilitySystemComponent* Source, const FGameplayEffectSpec& Spec, FActiveGameplayEffectHandle Handle);
	void						HandleEffectRemoved(const FGameplayEffectRemovalInfo& Info);
	void						HandleAttributeChanged(const FGameplayAttribute Attribute, float NewValue);
	void						HandleAbilityAdded(TSubclassOf<UAP_AbilityBase> Ability);
	void						HandleAbilityRemoved(TSubclassOf<UAP_AbilityBase> Ability);
	void						HandleTeamChanged(EGameTeam Team);
	void						HandleCloakStarted(ECloakingLevel Level);
	void						HandleCloakFinished(ECloakingLevel Level);
	void						HandleDeath();
	void						HandleLevelUp();
	UFUNCTION(BlueprintCallable)
	void						GrantBountyExp();
	void						Respawn();
	UFUNCTION(BlueprintCallable)
	bool						LineTraceUnit(int AbilitySlot, FVector Start, FVector Direction, AActor*& OutActor);
	UFUNCTION(BlueprintCallable)
	bool						LineTraceGround(int AbilitySlot, FVector Start, FVector Direction, FVector& OutLocation);
public:
	FDeathEventDelegate			OnDeath;
	FRespawnEventDelegate		OnRespawn;
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = FightUnit, meta = (AllowPrivateAccess = "true"))
		UAbilitySystemComponent*	AbilitySystem;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = FightUnit, meta = (AllowPrivateAccess = "true"))
		UAbilityUser*				AbilityUser;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = FightUnit, meta = (AllowPrivateAccess = "true"))
		UItemUser*					ItemUser;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = FightUnit, meta = (AllowPrivateAccess = "true"))
		UTeamAgent*					TeamAgent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = FightUnit)
		UAP_AttributeSet*			Stats;
protected:
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = Abilities)
		FGameplayTagContainer		GameplayTags;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Abilities)
		UDataTable*					StartingStats;
};
