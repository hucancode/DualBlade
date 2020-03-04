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

UENUM(BlueprintType)
enum class EFightStyle : uint8
{
	BareHand = 0,
	Axe = 1,
	AxeMelee = 2,
	Bow = 3,
	Dagger = 4,
	DualSword = 5,
	Katana1 = 6,
	Katana2 = 7,
	Katana3 = 8,
	MagicStaff = 9,
	SwordAndShield = 10,
	SwordNoShield = 11,
	SwordMelee = 12
};

UENUM(BlueprintType)
enum class EBodyType : uint8
{
	Male = 0,
	Female = 1
};

UCLASS()
class LINHIBLADE_API AAP_FightUnit : public ACharacter, public IAbilitySystemInterface, public IGameplayTagAssetInterface, public IGenericTeamAgentInterface
{
	GENERATED_BODY()

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDeathEventDelegate, const float, DeathTime);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FGeneralEventDelegate);
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
	UFUNCTION()
	void						HandleAbilityStateChanged();
	UFUNCTION()
	void						HandleAbilitySlotChanged();
	
	UFUNCTION()
	void						HandleEffectApplied(UAbilitySystemComponent* Source, const FGameplayEffectSpec& Spec, FActiveGameplayEffectHandle Handle);
	UFUNCTION()
	void						HandleEffectRemoved(const FGameplayEffectRemovalInfo& Info);
	UFUNCTION()
	void						HandleAttributeChanged(const FGameplayAttribute Attribute, float NewValue);
	UFUNCTION()
	void						HandleAbilityAdded(TSubclassOf<UAP_AbilityBase> Ability);
	UFUNCTION()
	void						HandleAbilityRemoved(TSubclassOf<UAP_AbilityBase> Ability);
	UFUNCTION()
	void						HandleWeaponChanged(); 
	UFUNCTION()
	void						HandleTeamChanged(EGameTeam Team);
	UFUNCTION()
	void						HandleCloakStarted(ECloakingLevel Level);
	UFUNCTION()
	void						HandleCloakFinished(ECloakingLevel Level);
	UFUNCTION()
	void						HandleDeath();
	UFUNCTION()
	void						HandleLevelUp();
	
	UFUNCTION(BlueprintCallable)
	void						GrantBountyExp();
	UFUNCTION(BlueprintCallable)
	void						LevelUpAbility(int AbilitySlot);
	UFUNCTION(BlueprintPure)
	bool						IsDead();
	UFUNCTION(BlueprintCallable)
	void						Respawn();
	UFUNCTION(BlueprintCallable)
	bool						LineTraceUnitAuto(ETargetingPolicy Targeting, FVector Start, FVector Direction, AActor*& OutActor);
	UFUNCTION(BlueprintCallable)
	bool						LineTraceAlly(FVector Start, FVector Direction, AActor*& OutActor, bool IgnoreMe);
	UFUNCTION(BlueprintCallable)
	bool						LineTraceEnemy(FVector Start, FVector Direction, AActor*& OutActor);
	UFUNCTION(BlueprintCallable)
	bool						LineTraceUnit(FVector Start, FVector Direction, AActor*& OutActor, bool IgnoreMe = true);
	UFUNCTION(BlueprintCallable)
	bool						LineTraceGround(FVector Start, FVector Direction, FVector& OutLocation);
	UFUNCTION(BlueprintCallable)
	void						GetAllUnitInRange(TArray<AActor*>& Result, float Radius, bool IgnoreMe = true);
	UFUNCTION(BlueprintCallable)
	void						GetAllEnemyInRange(TArray<AActor*>& Result, float Radius);
	UFUNCTION(BlueprintCallable)
	void						GetAllAllyInRange(TArray<AActor*>& Result, float Radius);
	UFUNCTION(BlueprintCallable)
	void						UseNewFightStyle(EFightStyle NewStyle);
public:
	UPROPERTY(BlueprintAssignable)
		FDeathEventDelegate			OnDeath;
	UPROPERTY(BlueprintAssignable)
		FGeneralEventDelegate		OnRespawn;
	UPROPERTY(BlueprintAssignable)
		FGeneralEventDelegate		OnFightStyleChanged;
public:
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
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = FightUnit)
		EFightStyle					FightStyle;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FightUnit)
		EBodyType					BodyType;
protected:
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = Abilities)
		FGameplayTagContainer		GameplayTags;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Abilities)
		UDataTable*					StartingStats;
	FTimerHandle					DeathTimerHandle;
};
