// Fill out your copyright notice in the Description page of Project Settings.

#include "AP_Hero.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Engine/StaticMesh.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "AbilitySystemComponent.h"
#include "Abilities/GameplayAbilityTargetActor.h"
#include "AbilitySystemBlueprintLibrary.h"

// Sets default values
AAP_Hero::AAP_Hero()
{
	// Set size for player capsule
	GetCapsuleComponent()->InitCapsuleSize(5.f, 90.0f);

	SelectionRing = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SelectionRing"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("StaticMesh'/Engine/BasicShapes/Plane'"));
	SelectionRing->SetStaticMesh(MeshAsset.Object);
	SelectionRing->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	SelectionRing->SetCollisionProfileName(TEXT("NoCollision"));
	SelectionRing->SetVisibility(false);
	SelectionRing->SetupAttachment(GetCapsuleComponent());
	SelectionRing->SetRelativeLocation(FVector(0.0f, 0.0f, -80.0f));

	SelectionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("SelectionBox"));
	SelectionBox->SetBoxExtent(FVector(32.0f, 32.0f, 80.0f));
	SelectionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	SelectionBox->SetupAttachment(GetCapsuleComponent());
	SelectionBox->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
	

	// Don't rotate character to camera direction
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Rotate character to moving direction
	GetCharacterMovement()->RotationRate = FRotator(0.f, 640.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;
	

	// Our ability system component.
	AbilitySystem = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystem"));
	AbilitySystem->SetIsReplicated(true);
	AbilitySystem->OnGameplayEffectAppliedDelegateToSelf.AddUObject(this, &AAP_Hero::OnGameplayEffectAppliedToSelf);
	
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	SpellAbilitySet = 0;
	AttributeSet = CreateDefaultSubobject<UAP_AttributeSet>(TEXT("AttributeSet"));
	/*UE_LOG(LogTemp, Warning, TEXT("CreateDefaultSubobject<UAP_AttributeSet>(); %x AttributeSet.GetAttackPower() %f"), AttributeSet, AttributeSet->GetAttackPower());*/
	bStatsInitialized = false;
	bAbilitiesInitialized = false;
	/*{
		TArray<UObject*> ChildObjects;
		TArray<UObject*> ASetObjects;
		GetObjectsWithOuter(this, ChildObjects, false, RF_NoFlags, EInternalObjectFlags::PendingKill);
		for (UObject* Obj : ChildObjects)
		{
			UAttributeSet* Set = Cast<UAttributeSet>(Obj);
			if (Set)
			{
				ASetObjects.AddUnique(Set);
			}
		}
		UE_LOG(LogTemp, Warning, TEXT("AAP_Hero::AAP_Hero() ChildObjects %d, ASetObjects %d"),
			ChildObjects.Num(), ASetObjects.Num());
	}*/
	ChannelEffectCount = 0;
}

// Called when the game starts or when spawned
void AAP_Hero::BeginPlay()
{
	Super::BeginPlay();
	/*UE_LOG(LogTemp, Warning, TEXT("AAP_Hero::BeginPlay() AttributeSet %x"), 
		AttributeSet);
	{
		TArray<UObject*> ChildObjects;
		TArray<UObject*> ASetObjects;
		GetObjectsWithOuter(this, ChildObjects, false, RF_NoFlags, EInternalObjectFlags::PendingKill);
		for (UObject* Obj : ChildObjects)
		{
			UAttributeSet* Set = Cast<UAttributeSet>(Obj);
			if (Set)
			{
				ASetObjects.AddUnique(Set);
			}
		}
		UE_LOG(LogTemp, Warning, TEXT("AAP_Hero::BeginPlay() ChildObjects %d, ASetObjects %d"),
			ChildObjects.Num(), ASetObjects.Num());
	}*/
	SetupAbilities();
	SetupStats();
	AbilitySystem->AbilityCommittedCallbacks.AddUObject(this, &AAP_Hero::OnAbilityCommitted);
	AbilitySystem->AbilityEndedCallbacks.AddUObject(this, &AAP_Hero::OnAbilityEnded);
	AbilitySystem->AbilityActivatedCallbacks.AddUObject(this, &AAP_Hero::OnAbilityActivated);
	
	if (!GetController()->IsValidLowLevel())
	{
		SpawnDefaultController();
	}
}

void AAP_Hero::SetupAbilities()
{
	if (Role == ROLE_Authority && !bAbilitiesInitialized)
	{
		int32 Level = 1;
		if (!SpellAbilitySet)
		{
			return;
		}
		GiveAbility(Level, SpellAbilitySet->Abilities[ESpellSlot::Ability1]);
		GiveAbility(Level, SpellAbilitySet->Abilities[ESpellSlot::Ability2]);
		GiveAbility(Level, SpellAbilitySet->Abilities[ESpellSlot::Ability3]);
		GiveAbility(Level, SpellAbilitySet->Abilities[ESpellSlot::Ability4]);
		{
			// R Job
			ESpellSlot slot = ESpellSlot::Ability5;
			switch (Job)
			{
			case EJob::Job1:
				slot = ESpellSlot::Ability5;
				break;
			case EJob::Job2:
				slot = ESpellSlot::Ability6;
				break;
			case EJob::Job3:
				slot = ESpellSlot::Ability7;
				break;
			case EJob::Job4:
				slot = ESpellSlot::Ability8;
				break;
			case EJob::Job5:
				slot = ESpellSlot::Ability9;
				break;
			default:
				break;
			}
			GiveAbility(Level, SpellAbilitySet->Abilities[slot]);
		}
		// we used to activate passive skill through this, but now we don't
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, FGameplayTag::RequestGameplayTag("Event.TriggerPassiveEffect"), FGameplayEventData());
		bAbilitiesInitialized = true;
	}
}

bool AAP_Hero::IsBuffPresent(const TSubclassOf<UGameplayEffect> Effect)
{
	return GetBuffStack(Effect) > 0;
}

int AAP_Hero::GetBuffStack(const TSubclassOf<UGameplayEffect> Effect)
{
	FGameplayEffectQuery Query;
	Query.EffectDefinition = Effect;
	return AbilitySystem->GetAggregatedStackCount(Query);
}

void AAP_Hero::SetupStats()
{
	if (Role == ROLE_Authority && !bStatsInitialized)
	{
		/*UE_LOG(LogTemp, Warning, TEXT("setup stats, starting..."));*/
		FGameplayEffectContextHandle EffectContext = AbilitySystem->MakeEffectContext();
		EffectContext.AddSourceObject(this);

		FGameplayEffectSpecHandle NewHandle = AbilitySystem->MakeOutgoingSpec(BaseStats, GetCharacterLevel(), EffectContext);
		if (NewHandle.IsValid())
		{
			FActiveGameplayEffectHandle ActiveGEHandle = AbilitySystem->ApplyGameplayEffectSpecToTarget(*NewHandle.Data.Get(), AbilitySystem);
			bStatsInitialized = true;
			/*UE_LOG(LogTemp, Warning, TEXT("setup stats, done AttributeSet %x"), AttributeSet);
			TArray< FGameplayAttribute> all;
			AbilitySystem->GetAllAttributes(all);
			UE_LOG(LogTemp, Warning, TEXT("setup stats, done AbilitySystem->GetAllAttributes() %d"), all.Num());*/
		}
	}
}

void AAP_Hero::GiveAbility(const int32& Level, TSubclassOf<UGameplayAbility> Ability)
{
	FGameplayAbilitySpecHandle handle = AbilitySystem->GiveAbility(
		FGameplayAbilitySpec(Ability, Level, INDEX_NONE, this));
	SpellAbilityHandles.Add(handle);
	SpellStates.AddDefaulted();
}

void AAP_Hero::RemoveAllAbilities()
{
	if (Role == ROLE_Authority)
	{
		SpellAbilityHandles.Empty();
		SpellStates.Empty();
		AbilitySystem->CancelAllAbilities();
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, 
			FGameplayTag::RequestGameplayTag("Event.TriggerPassiveDetach"), FGameplayEventData());
		// potential crash here (known bug, but still have't found any effective fix)
		// basically you need to make sure no skill are in the middle of activation before remove them
		AbilitySystem->ClearAllAbilities();
		bAbilitiesInitialized = false;
		
	}
}

void AAP_Hero::SpellAttack(int SpellSlot)
{
	bool valid = SpellAbilityHandles.IsValidIndex(SpellSlot);
	UE_LOG(LogTemp, Warning, TEXT("about to activate ability %d, valid=%d"), SpellSlot, valid);
	if (valid && AbilitySystem && bStatsInitialized && bAbilitiesInitialized)
	{
		/*float x = AbilitySystem->GetNumericAttribute(AttributeSet->GetManaAttribute());
		float y = AttributeSet->GetManaAttribute().GetNumericValue(AttributeSet);
		UE_LOG(LogTemp, Warning, TEXT("activate ability, x = %f, y = %f"), x, y);*/
		// If bAllowRemoteActivation is true, it will remotely activate local / server abilities, if false it will only try to locally activate the ability
		bool ret = AbilitySystem->TryActivateAbility(SpellAbilityHandles[SpellSlot], true);
		if (ret)
		{
			SpellStates[SpellSlot] = ESpellState::Casting;
			SpellCastDelegate.Broadcast(SpellSlot);
		}
		UE_LOG(LogTemp, Warning, TEXT("activate ability, ret = %d"), ret);
	}
}

void AAP_Hero::CancelAllAttack()
{
	if (AbilitySystem)
	{
		AbilitySystem->CancelAllAbilities();
		UE_LOG(LogTemp, Warning, TEXT("cancel all ability"));
	}
}

void AAP_Hero::RemoveAllChannelingEffect()
{
	if (AbilitySystem)
	{
		if (ChannelEffectCount)
		{
			static FGameplayTagContainer Container =
				FGameplayTag::RequestGameplayTag("Combat.Channeling").GetSingleTagContainer();
			AbilitySystem->RemoveActiveEffectsWithGrantedTags(Container);
			UE_LOG(LogTemp, Warning, TEXT("cancel all channeling ability"));
			ChannelEffectCount = 0;
		}
	}
}

float AAP_Hero::GetSpellCooldown(int SpellSlot)
{
	bool valid = SpellAbilityHandles.IsValidIndex(SpellSlot);
	UE_LOG(LogTemp, Warning, TEXT("about to check ability cooldown %d, valid=%d"), SpellSlot, valid);
	float ret = 0.0f;
	if (valid && AbilitySystem)
	{
		if (!AbilitySystem->GetActivatableAbilities().IsValidIndex(SpellSlot))
		{
			return ret;
		}
		if (!AbilitySystem->GetActivatableAbilities()[SpellSlot].GetAbilityInstances().Num())
		{
			return ret;
		}
		UGameplayAbility* ability = AbilitySystem->GetActivatableAbilities()[SpellSlot].GetAbilityInstances().Last();
		ret = ability->GetCooldownTimeRemaining();
	}
	UE_LOG(LogTemp, Warning, TEXT("return %f"), ret);
	return ret;
}

float AAP_Hero::GetSpellCooldownPercent(int SpellSlot)
{
	bool valid = SpellAbilityHandles.IsValidIndex(SpellSlot)
		&& AbilitySystem->GetActivatableAbilities().IsValidIndex(SpellSlot)
		&& AbilitySystem->GetActivatableAbilities()[SpellSlot].GetAbilityInstances().Num();
	
	UE_LOG(LogTemp, Warning, TEXT("about to check ability cooldown percent %d, valid=%d"), SpellSlot, valid);
	float ret = 0.0f;
	if (valid && AbilitySystem)
	{
		UGameplayAbility* ability = AbilitySystem->GetActivatableAbilities()[SpellSlot].GetAbilityInstances().Last();
		FGameplayAbilitySpecHandle handle = SpellAbilityHandles[SpellSlot];
		float remaining;
		float duration;
		ability->GetCooldownTimeRemainingAndDuration(handle, ability->GetCurrentActorInfo(), remaining, duration);
		if (duration == 0.0f)
		{
			UE_LOG(LogTemp, Warning, TEXT("duration = 0, return %f"), ret);
			return ret;
		}
		ret = remaining / duration;
	}
	UE_LOG(LogTemp, Warning, TEXT("return %f"), ret);
	return ret;
}

ESpellState AAP_Hero::GetSpellState(int SpellSlot)
{
	if (!SpellStates.IsValidIndex(SpellSlot))
	{
		return ESpellState::Disabled;
	}
	return SpellStates[SpellSlot];
}

// Called every frame
void AAP_Hero::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (!GetCharacterMovement()->Velocity.IsNearlyZero())
	{
		UE_LOG(LogTemp, Warning, TEXT("AAP_Hero::Tick is moving = true"));
		RemoveAllChannelingEffect();
	}
}

// Called to bind functionality to input
void AAP_Hero::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	//AbilitySystem->BindToInputComponent(PlayerInputComponent);// we will do this manually in blueprint
}

void AAP_Hero::PossessedBy(AController * NewController)
{
	Super::PossessedBy(NewController);

	AbilitySystem->RefreshAbilityActorInfo();
}

void AAP_Hero::OnRep_Controller()
{
	Super::OnRep_Controller();

	// Our controller changed, must update ActorInfo on AbilitySystemComponent
	if (AbilitySystem)
	{
		AbilitySystem->RefreshAbilityActorInfo();
	}
}

void AAP_Hero::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//DOREPLIFETIME(AAP_Hero, CharacterLevel);
}

UAbilitySystemComponent * AAP_Hero::GetAbilitySystemComponent() const
{
	return AbilitySystem;
}

void AAP_Hero::OnGameplayEffectAppliedToSelf(UAbilitySystemComponent * Source, const FGameplayEffectSpec & Spec, FActiveGameplayEffectHandle Handle)
{
	UE_LOG(LogTemp, Warning, TEXT("AAP_Hero::OnGameplayEffectAppliedToSelf duration = %f"), Spec.GetDuration());
	GameplayEffectAppliedToSelf.Broadcast(Source, Spec, Handle);
	FOnActiveGameplayEffectRemoved_Info* Delegate = AbilitySystem->OnGameplayEffectRemoved_InfoDelegate(Handle);
	if (Delegate)
	{
		Delegate->AddUObject(this, &AAP_Hero::OnGameplayEffectRemovedFromSelf);
	}
	FGameplayTagContainer Container;
	Spec.GetAllGrantedTags(Container);
	if (Container.HasTag(FGameplayTag::RequestGameplayTag("Combat.Channeling")))
	{
		ChannelEffectCount++;
	}
}

void AAP_Hero::OnGameplayEffectRemovedFromSelf(const FGameplayEffectRemovalInfo & Info)
{
	FGameplayTagContainer Container;
	FGameplayTagContainer Container_Dummy;
	Info.EffectContext.GetOwnedGameplayTags(Container_Dummy, Container);
	if (Container.HasTag(FGameplayTag::RequestGameplayTag("Combat.Channeling")))
	{
		ChannelEffectCount--;
	}
	GameplayEffectRemovedFromSelf.Broadcast(Info);
}

void AAP_Hero::OnAbilityActivated(UGameplayAbility * Ability)
{
	UE_LOG(LogTemp, Warning, TEXT("AAP_Hero::OnAbilityActivated %s"), *Ability->GetFName().ToString());
	FGameplayAbilitySpecHandle Handle = Ability->GetCurrentAbilitySpecHandle();
	int Index = SpellAbilityHandles.Find(Handle);
	if (Index == INDEX_NONE)
	{
		return;
	}
	SpellStates[Index] = ESpellState::Casting;
	SpellCastDelegate.Broadcast(Index);
}

void AAP_Hero::OnAbilityCommitted(UGameplayAbility* Ability)
{
	UE_LOG(LogTemp, Warning, TEXT("AAP_Hero::OnAbilityCommitted %s"), *Ability->GetFName().ToString());
	FGameplayAbilitySpecHandle Handle = Ability->GetCurrentAbilitySpecHandle();
	int Index = SpellAbilityHandles.Find(Handle);
	if (Index == INDEX_NONE)
	{
		return;
	}
	if (!Ability->CheckCooldown(Handle, Ability->GetCurrentActorInfo()))
	{
		SpellStates[Index] = ESpellState::OnCooldown;
		SpellGoneCooldown.Broadcast(Index);
		FGameplayEffectQuery query;
		query.EffectDefinition = Ability->GetCooldownGameplayEffect()->GetClass();
		TArray<FActiveGameplayEffectHandle> Handles = AbilitySystem->GetActiveEffects(query);
		UE_LOG(LogTemp, Warning, TEXT("setting up delegate..."));
		if (Handles.Num())
		{
			FActiveGameplayEffectHandle CooldownEffect = Handles.Last();
			FOnActiveGameplayEffectRemoved_Info* Delegate = AbilitySystem->OnGameplayEffectRemoved_InfoDelegate(CooldownEffect);
			if (Delegate)
			{
				Delegate->AddUObject(this, &AAP_Hero::OnAbilityOffCooldown);
				UE_LOG(LogTemp, Warning, TEXT("delegate setup successful"));
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("can't find delegate, setup unsuccessful"));
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("can't find cooldown effect, setup unsuccessful"));
		}
	}
}

void AAP_Hero::OnAbilityEnded(UGameplayAbility* Ability)
{
	UE_LOG(LogTemp, Warning, TEXT("AAP_Hero::OnAbilityEnded %s"), *Ability->GetFName().ToString());
	FGameplayAbilitySpecHandle Handle = Ability->GetCurrentAbilitySpecHandle();
	int Index = SpellAbilityHandles.Find(Handle);
	if (Index == INDEX_NONE)
	{
		return;
	}
	if (!Ability->CheckCooldown(Handle, Ability->GetCurrentActorInfo()))
	{
		SpellStates[Index] = ESpellState::OnCooldown;
		SpellGoneCooldown.Broadcast(Index);
	}
	else
	{
		SpellStates[Index] = ESpellState::Ready;
		SpellOffCooldown.Broadcast(Index);
	}
}

void AAP_Hero::OnAbilityOffCooldown(const FGameplayEffectRemovalInfo& InGameplayEffectRemovalInfo)
{
	UE_LOG(LogTemp, Warning, TEXT("AAP_Hero::OnAbilityOffCooldown"));
	// this is not the actual ability we are concerning, just a default ability instance
	// weird but i don't know why
	const UGameplayAbility* AbilityInfo = InGameplayEffectRemovalInfo.EffectContext.GetAbility();
	ESpellSlot Slot = SpellAbilitySet->Find(AbilityInfo);
	if (Slot == ESpellSlot::Ability_Invalid)
	{
		UE_LOG(LogTemp, Warning, TEXT("an ability just gone off cooldown, but it seems doesn't belong to this actor %s/%s"), *GetFName().ToString(), *AbilityInfo->GetFName().ToString());
		return;
	}
	SpellStates[(int)Slot] = ESpellState::Ready;
	SpellOffCooldown.Broadcast((int)Slot);
}

void AAP_Hero::HandleHealthChanged(float NewValue)
{
	// We only call the BP callback if this is not the initial ability setup
	if (bStatsInitialized)
	{
		OnHealthChanged(NewValue);
	}
}

void AAP_Hero::HandleManaChanged(float NewValue)
{
	if (bStatsInitialized)
	{
		OnManaChanged(NewValue);
	}
}

void AAP_Hero::HandleMoveSpeedChanged(float NewValue)
{
	UE_LOG(LogTemp, Warning, TEXT("AAP_Hero::HandleMoveSpeedChanged"));
	if (bStatsInitialized)
	{
		GetCharacterMovement()->MaxWalkSpeed = NewValue;
	}
}

void AAP_Hero::HandleTurnRateChanged(float NewValue)
{
	UE_LOG(LogTemp, Warning, TEXT("AAP_Hero::HandleTurnRateChanged"));
	if (bStatsInitialized)
	{
		GetCharacterMovement()->RotationRate.Yaw = NewValue;
	}
}

void AAP_Hero::SelectHero(bool selected)
{
	IsSelected = selected;
	SelectionRing->SetVisibility(IsSelected);
}

float AAP_Hero::GetHealth() const
{
	return AttributeSet->GetHealth();
}

float AAP_Hero::GetMaxHealth() const
{
	return AttributeSet->GetMaxHealth();
}

float AAP_Hero::GetMana() const
{
	return AttributeSet->GetMana();
}

float AAP_Hero::GetMaxMana() const
{
	return AttributeSet->GetMaxMana();
}

int32 AAP_Hero::GetCharacterLevel() const
{
	return CharacterLevel;
}

int32 AAP_Hero::GetSpellCount() const
{
	return SpellAbilityHandles.Num();
}

bool AAP_Hero::IsTargeting() const
{
	for (AGameplayAbilityTargetActor* Actor : AbilitySystem->SpawnedTargetActors)
	{
		if (!Actor) continue;
		if (Actor->IsActorBeingDestroyed()) continue;
		if (!Actor->IsConfirmTargetingAllowed()) continue;
		return true;
	}
	return false;
}

bool AAP_Hero::SetCharacterLevel(int32 NewLevel)
{
	if (CharacterLevel != NewLevel && NewLevel > 0)
	{
		CharacterLevel = NewLevel;
		return true;
	}
	return false;
}

void AAP_Hero::EnterVanish()
{
	OnVanishedStarted();
}

void AAP_Hero::QuitVanish()
{
	OnVanishedFinished();
}

void AAP_Hero::EnterCloak()
{
	OnCloakStarted();
}

void AAP_Hero::QuitCloak()
{
	OnCloakFinished();
}

void AAP_Hero::EnterInvi()
{
	OnInviStarted();
}

void AAP_Hero::QuitInvi()
{
	OnInviFinished();
}
