// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "AP_AttributeSet.h"
#include "CoreMinimal.h"
#include "Delegates/IDelegateInstance.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"
#include "TimerManager.h"
#include "GameplayAbility.h"
#include "GenericTeamAgentInterface.h"
#include <AP_GameplayAbilitySet.h>
#include "AP_Hero.generated.h"


UENUM(BlueprintType)
enum class EAbilityState : uint8
{
	Ready 		UMETA(DisplayName = "Ready"),
	Casting 	UMETA(DisplayName = "Casting"),
	OnCooldown	UMETA(DisplayName = "OnCooldown"),
	Disabled	UMETA(DisplayName = "Disabled")
};

UENUM(BlueprintType)
enum class EJob : uint8
{
	Job1 		UMETA(DisplayName = "Warrior"),
	Job2 		UMETA(DisplayName = "Summonner"),
	Job3		UMETA(DisplayName = "Fighter"),
	Job4		UMETA(DisplayName = "Mechanic"),
	Job5		UMETA(DisplayName = "Assasin"),
};
UENUM(BlueprintType)
enum class ECloakingLevel : uint8
{
	None		UMETA(DisplayName = "None"),
	Cloaked		UMETA(DisplayName = "Cloaked"),
	Invisible	UMETA(DisplayName = "Invisible"),
	Vanished	UMETA(DisplayName = "Vanished"),
};

#define EXP_AQUIRING_RANGE 800.0f

UCLASS()
class LINHIBLADE_API AAP_Hero : public ACharacter, public IAbilitySystemInterface, public IGameplayTagAssetInterface, public IGenericTeamAgentInterface
{
	GENERATED_BODY()

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FStatEventDelegate);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAbilityEventDelegate, int, AbilitySlot);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnGameplayEffectAppliedDelegate, UAbilitySystemComponent*, Source, const FGameplayEffectSpec&, Spec, FActiveGameplayEffectHandle, Handle);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGameplayEffectRemovedDelegate, const FGameplayEffectRemovalInfo&, Info);

public:
	// Sets default values for this character's properties
	AAP_Hero();
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void PossessedBy(AController * NewController);
	virtual void OnRep_Controller() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	virtual void SetGenericTeamId(const FGenericTeamId& Id) override;
	virtual FGenericTeamId GetGenericTeamId() const override;
	
	/** select hero, activate selection effect */
	UFUNCTION(BlueprintCallable, Category = "Abilities")
		void SelectHero(bool selected);

	/** Returns current health, will be 0 if dead */
	UFUNCTION(BlueprintCallable, Category = "Abilities")
		float GetHealthPercent() const;

	/** Returns current mana */
	UFUNCTION(BlueprintCallable, Category = "Abilities")
		float GetManaPercent() const;

	/** Returns the character level that is passed to the ability system */
	UFUNCTION(BlueprintCallable, Category = "Abilities")
		int32 GetAbilityCount() const;
	/** Returns the character level that is passed to the ability system */
	UFUNCTION(BlueprintCallable, Category = "Abilities")
		UGameplayAbility* GetAbility(int AbilitySlot) const;
	/** Returns the character level that is passed to the ability system */
	UFUNCTION(BlueprintCallable, Category = "Abilities")
		bool CanActivateAbility(int AbilitySlot) const;
	/** Returns the character level that is passed to the ability system */
	UFUNCTION(BlueprintCallable, Category = "Abilities")
		bool LevelUpAbility(int AbilitySlot);
	/** Returns the character level that is passed to the ability system */
	UFUNCTION(BlueprintCallable, Category = "Progress")
		void GiveExp(float Amount);

protected:
	void SetupStats();
	void GiveAbility(const int32& Level, TSubclassOf<UGameplayAbility> Ability);
	/** Apply the startup gameplay abilities and effects */
	UFUNCTION(BlueprintCallable, Category = "Abilities")
		void SetupAbilities();
	/** Apply the startup gameplay abilities and effects */
	UFUNCTION(BlueprintCallable, Category = "Abilities")
		bool IsBuffPresent(const TSubclassOf<UGameplayEffect> Effect);
	/** Apply the startup gameplay abilities and effects */
	UFUNCTION(BlueprintCallable, Category = "Abilities")
		int GetBuffStack(const TSubclassOf<UGameplayEffect> Effect);
	/** Apply the startup gameplay abilities and effects */
	UFUNCTION(BlueprintCallable, Category = "Abilities")
		void RemoveAllAbilities();
	/** Apply the startup gameplay abilities and effects */
	UFUNCTION(BlueprintCallable, Category = "Abilities")
		int GetAbilityLevel(int AbilitySlot);
	/** Apply the startup gameplay abilities and effects */
	UFUNCTION(BlueprintCallable, Category = "Abilities")
		void SetAbilityLevel(int AbilitySlot, int Level);
	/** Apply the startup gameplay abilities and effects */
	UFUNCTION(BlueprintCallable, Category = "Abilities")
		void ActivateAbility(int AbilitySlot);
	/** Apply the startup gameplay abilities and effects */
	UFUNCTION(BlueprintCallable, Category = "Abilities")
		void CancelAllAttack();
	/** Apply the startup gameplay abilities and effects */
	UFUNCTION(BlueprintCallable, Category = "Abilities")
		void RemoveAllChannelingEffect();
	UFUNCTION(BlueprintCallable, Category = "Abilities")
		float GetAbilityCooldown(int AbilitySlot);
	UFUNCTION(BlueprintCallable, Category = "Abilities")
		float GetAbilityCooldownPercent(int AbilitySlot);
	UFUNCTION(BlueprintCallable, Category = "Abilities")
		EAbilityState GetAbilityState(int AbilitySlot);
	UFUNCTION(BlueprintCallable, Category = "Abilities")
		bool GetAbilityAutoCastEnabled(int AbilitySlot);
	UFUNCTION(BlueprintCallable, Category = "Abilities")
		bool IsAbilityBusy(int AbilitySlot);
	UFUNCTION(BlueprintCallable, Category = "Abilities")
		void GetAllUnitInRange(TArray<AAP_Hero*>& Result, float Radius);
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	/**
	 * Called when health is changed, either from healing or from being damaged
	 * For damage this is called in addition to OnDamaged/OnKilled
	 *
	 * @param DeltaValue Change in health value, positive for heal, negative for cost. If 0 the delta is unknown
	 * @param EventTags The gameplay tags of the event that changed mana
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Abilities")
		void OnHealthChanged(float NewValue);
	UFUNCTION(BlueprintImplementableEvent, Category = "Abilities")
		void OnDeath(float DeathTime);
	UFUNCTION(BlueprintImplementableEvent, Category = "Abilities")
		void OnRespawn();
	/**
	 * Called when mana is changed, either from healing or from being used as a cost
	 *
	 * @param DeltaValue Change in mana value, positive for heal, negative for cost. If 0 the delta is unknown
	 * @param EventTags The gameplay tags of the event that changed mana
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Abilities")
		void OnManaChanged(float NewValue);
	UFUNCTION(BlueprintImplementableEvent, Category = "Abilities")
		void OnLevelChanged(float NewValue);
	UFUNCTION(BlueprintImplementableEvent, Category = "Abilities")
		void OnExpChanged(float NewValue);
	
	UFUNCTION(BlueprintImplementableEvent, Category = "Abilities")
		void OnCloakStarted(ECloakingLevel Level);

	UFUNCTION(BlueprintImplementableEvent, Category = "Abilities")
		void OnCloakFinished(ECloakingLevel LastCloakingLevel);

	UFUNCTION(BlueprintImplementableEvent, Category = "AI")
		void OnTeamUpdated(const FGenericTeamId& NewTeam);

	/** Called when a game effect applied to self
	 *
	 * @param Source who are giving the effect
	 * @param Spec effect information
	 * @param Handle which effect are being applied
	 */
	UFUNCTION(Category = "Abilities")
		void OnGameplayEffectAppliedToSelf(UAbilitySystemComponent* Source, const FGameplayEffectSpec& Spec, FActiveGameplayEffectHandle Handle);

	/** Called when a game effect applied to self
	 *
	 * @param Source who are giving the effect
	 * @param Spec effect information
	 * @param Handle which effect are being applied
	 */
	UFUNCTION(Category = "Abilities")
		void OnGameplayEffectRemovedFromSelf(const FGameplayEffectRemovalInfo& Info);


	/** Called when an ability is activated
	 *
	 * @param Ability which ability are being activated
	 */
	UFUNCTION(Category = "Abilities")
		void OnAbilityActivated(UGameplayAbility* Ability);

	/** Called when an ability is committed
	 *
	 * @param Ability which ability are being committed
	 */
	UFUNCTION(Category = "Abilities")
		void OnAbilityCommitted(UGameplayAbility* Ability);
	
	/** Called when an ability is ended
	 *
	 * @param Ability which ability are being ended
	 */
	UFUNCTION(Category = "Abilities")
		void OnAbilityEnded(UGameplayAbility* Ability);

	/** Called when an ability off cooldown
	 *
	 * @param InGameplayEffectRemovalInfo detail about the event
	 */
	UFUNCTION(Category = "Abilities")
		void OnAbilityOffCooldown(const FGameplayEffectRemovalInfo& InGameplayEffectRemovalInfo);
	

	void GetAllEnemyInRange();
	void GrantBountyExp();

	// Called from RPGAllStats, these call BP events above
	void HandleHealthChanged(float NewValue);
	void HandleManaChanged(float NewValue);
	void HandleLevelChanged(float NewValue);
	void HandleExpChanged(float NewValue);
	void HandleMoveSpeedChanged(float NewValue);
	void HandleTurnRateChanged(float NewValue);

	// Friended to allow access to handle functions above
	friend UAP_AttributeSet;

	/**
	 * Called when character cast a skill
	 *
	 * @param AbilitySlot Which skill are being cast
	 */
	UPROPERTY(BlueprintAssignable)
		FAbilityEventDelegate AbilityCastDelegate;
	/**
	 * Called when a spell gone cooldown
	 *
	 * @param AbilitySlot Which skill are being cast
	 */
	UPROPERTY(BlueprintAssignable)
		FAbilityEventDelegate AbilityGoneCooldown;
	/**
	 * Called when a spell off cooldown
	 *
	 * @param AbilitySlot Which skill is just off cooldown
	 */
	UPROPERTY(BlueprintAssignable)
		FAbilityEventDelegate AbilityOffCooldown;
	/**
	 * Called when a spell off cooldown
	 *
	 * @param AbilitySlot Which skill is just off cooldown
	 */
	UPROPERTY(BlueprintAssignable)
		FAbilityEventDelegate AbilityLevelUp;
	/**
	 * Called when a spell off cooldown
	 *
	 * @param AbilitySlot Which skill is just off cooldown
	 */
	UPROPERTY(BlueprintAssignable)
		FStatEventDelegate AbilityPointChange;

	/**
	 * Called when a gamplay effect applied to self
	 *
	 * @param GameplayEffectSpec Which effect are being applied
	 * @param Handle The handle for the effect itself
	 */
	UPROPERTY(BlueprintAssignable)
		FOnGameplayEffectAppliedDelegate GameplayEffectAppliedToSelf;
	UPROPERTY(BlueprintAssignable)
		FOnGameplayEffectRemovedDelegate GameplayEffectRemovedFromSelf;
protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Abilities, meta = (AllowPrivateAccess = "true"))
		class UStaticMeshComponent* SelectionRing;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Abilities, meta = (AllowPrivateAccess = "true"))
		class UBoxComponent* SelectionBox;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Abilities)
		bool IsSelected;

	UPROPERTY()
		TArray <EAbilityState> AbilityStates;

	/** Our ability system */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Abilities, meta = (AllowPrivateAccess = "true"))
		UAbilitySystemComponent* AbilitySystem;

	/** List of attributes modified by the ability system */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Abilities)
		UAP_AttributeSet* AllStats;

	/** Abilities to grant to this character on creation. These will be activated by tag or event and are not bound to specific inputs */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Abilities)
		UAP_GameplayAbilitySet* AbilitySet;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Abilities)
		EJob Job;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Abilities)
		TSubclassOf<UGameplayEffect> BaseStats;

	UPROPERTY()
		TArray<FGameplayAbilitySpecHandle> AbilityHandles;
	UPROPERTY()
		int ChannelEffectCount;
	/** If true we have initialized our abilities */
	UPROPERTY()
		int32 bAbilitiesInitialized;
	UPROPERTY()
		int32 bStatsInitialized;
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Gameplay Tags")
		FGameplayTagContainer GameplayTags;
	FGenericTeamId TeamId;
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly)
		AController* LogicalController;
	UPROPERTY()
		ECloakingLevel CloakStatus;
public:
	UFUNCTION(BlueprintCallable, NetMulticast, Reliable, Category = "Abilities")
		void EnterCloak(ECloakingLevel Level);

	UFUNCTION(BlueprintCallable, NetMulticast, Reliable, Category = "Abilities")
		void QuitCloak();
	UFUNCTION(BlueprintCallable, Category = "Abilities")
		bool IsVisibleToTeam(FGameplayTag TeamTag);

	UFUNCTION(BlueprintCallable, NetMulticast, Reliable, Category = Abilities)
		void EnterDeath();

	UFUNCTION(BlueprintCallable, NetMulticast, Reliable, Category = Abilities)
		void Respawn();

	UFUNCTION(BlueprintCallable, Category = GameplayTags)
		bool RemoveGameplayTag(FGameplayTag Tag);
	UFUNCTION(BlueprintCallable, Category = GameplayTags)
		void AddGameplayTag(FGameplayTag Tag);

	UFUNCTION(BlueprintCallable, Category = GameplayTags)
		virtual void GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const override;
	UFUNCTION(BlueprintCallable, Category = GameplayTags)
		virtual bool HasMatchingGameplayTag(FGameplayTag TagToCheck) const;
	UFUNCTION(BlueprintCallable, Category = GameplayTags)
		virtual bool HasAllMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const;
	UFUNCTION(BlueprintCallable, Category = GameplayTags)
		virtual bool HasAnyMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const;
	UFUNCTION(BlueprintCallable)
		bool IsLogicalController(AController* OtherController);
	UFUNCTION(BlueprintCallable)
		void SetLogicalController(AController* NewController);
	// blueprint proxy function
	UFUNCTION(BlueprintCallable)
		void SetTeam(const EGameTeam Id);
	// blueprint proxy function
	UFUNCTION(BlueprintCallable)
		EGameTeam GetTeam();
	// blueprint proxy function
	UFUNCTION(BlueprintCallable)
		bool IsAlly();
	UFUNCTION(BlueprintCallable)
		void SetJob(EJob NewJob);
};