// Fill out your copyright notice in the Description page of Project Settings.

#include "AP_Hero.h"
#include "Engine/World.h"
#include "UObject/ConstructorHelpers.h"
#include "AIController.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Engine/StaticMesh.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "Net/UnrealNetwork.h"
#include "Abilities/GameplayAbilityTargetActor.h"
#include "AbilitySystemBlueprintLibrary.h"
#include <LinhiBlade/AP_GameMode.h>
#include <AP_AIController.h>
#include <AP_RTSPlayerController.h>
#include "DrawDebugHelpers.h"

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

	AllStats_ = CreateDefaultSubobject<UAP_AttributeSet>(TEXT("AllStats_"));
	UE_LOG_FAST(TEXT("AAP_Hero::AAP_Hero(), AllStats_=%x"), AllStats_);

	bStatsInitialized = false;
	ChannelEffectCount = 0;
	CurrentWeapon_ = EWeaponCategory::BareHand;
	SetReplicates(true);
}

// Called when the game starts or when spawned
void AAP_Hero::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG_FAST(TEXT("AAP_Hero::BeginPlay(), AllStats_=%x"), AllStats_);
	SetupStats();
	AbilitySystem->AbilityCommittedCallbacks.AddUObject(this, &AAP_Hero::OnAbilityCommitted);
	AbilitySystem->AbilityEndedCallbacks.AddUObject(this, &AAP_Hero::OnAbilityEnded);
	AbilitySystem->AbilityActivatedCallbacks.AddUObject(this, &AAP_Hero::OnAbilityActivated);
	if (!GetController()->IsValidLowLevel())
	{
		SpawnDefaultController();
	}
	if (GetGenericTeamId() == FGenericTeamId::NoTeam)
	{
		SetGenericTeamId((uint8)EGameTeam::Neutral);
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

void AAP_Hero::GiveExp(float Amount)
{
	UE_LOG_FAST(TEXT("AAP_Hero::GiveExp %f"), Amount);
	AllStats_->SetExperience(AllStats_->GetExperience() + Amount);
}

void AAP_Hero::SetupStats()
{
	if (!HasAuthority())
	{
		return;
	}
	if (bStatsInitialized)
	{
		return;
	}
	if (!StartingStats)
	{
		return;
	}
	if (!StartingStats->IsValidLowLevel())
	{
		return;
	}
	UE_LOG_FAST(TEXT("setup stats, starting..."));
	AllStats_->InitFromMetaDataTable(StartingStats);
	bStatsInitialized = true;
}

void AAP_Hero::GiveAbility(TSubclassOf<UGameplayAbility> Ability)
{
	if (!Ability->IsValidLowLevel())
	{
		UE_LOG_FAST(TEXT("AAP_Hero::GiveAbility failed"));
		return;
	}
	FGameplayAbilitySpec* spec;
	FGameplayAbilitySpecHandle handle;
	spec = AbilitySystem->FindAbilitySpecFromClass(Ability);
	if (!spec)
	{
		spec = new FGameplayAbilitySpec(Ability, 0, INDEX_NONE, this);
		handle = AbilitySystem->GiveAbility(*spec);
	}
	else
	{
		handle = spec->Handle;
	}
	if (AbilityHandles.Find(handle) != INDEX_NONE)
	{
		return;
	}
	AbilityHandles.Add(handle);
	AbilityStates.AddDefaulted();
	OnSkillChanged();
}

void AAP_Hero::RemoveAbility(TSubclassOf<UGameplayAbility> Ability)
{
	auto spec = AbilitySystem->FindAbilitySpecFromClass(Ability);
	int index = AbilityHandles.Find(spec->Handle);
	if (index == INDEX_NONE)
	{
		return;
	}
	AbilityHandles.RemoveAt(index);
	AbilityStates.RemoveAt(index);
	OnSkillChanged();
}

void AAP_Hero::RemoveAllAbilities()
{
	if (!HasAuthority())
	{
		return;
	}
	AbilityHandles.Empty();
	AbilityStates.Empty();
	AbilitySystem->CancelAllAbilities();
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, 
		FGameplayTag::RequestGameplayTag("Event.TriggerPassiveDetach"), FGameplayEventData());
	const auto AllAbility = AbilitySystem->GetActivatableAbilities();
	for (auto Ability : AllAbility)
	{
		AllStats_->SetAbilityPoint(AllStats_->GetAbilityPoint() + Ability.Level);
		Ability.Level = 0;// not sure if this works
	}
	// potential crash here (known bug, but still have't found any good fix)
	// basically you need to make sure no skill are in the middle of activation before remove them
	//AbilitySystem->ClearAllAbilities();
}

int AAP_Hero::GetAbilityLevel(int AbilitySlot)
{
	bool valid = AbilityHandles.IsValidIndex(AbilitySlot);
	if (valid && AbilitySystem)
	{
		auto Ability = AbilitySystem->FindAbilitySpecFromHandle(AbilityHandles[AbilitySlot]);
		if (Ability)
		{
			return Ability->Level;
		}
	}
	return 0;
}

void AAP_Hero::SetAbilityLevel(int AbilitySlot, int Level)
{
	bool valid = AbilityHandles.IsValidIndex(AbilitySlot);
	UE_LOG_FAST(TEXT("about to set ability level %d, valid=%d"), AbilitySlot, valid);
	if (!valid || !AbilitySystem)
	{
		return;
	}
	auto Ability = AbilitySystem->FindAbilitySpecFromHandle(AbilityHandles[AbilitySlot]);
	if (!Ability)
	{
		return;
	}
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, FGameplayTag::RequestGameplayTag("Event.TriggerPassiveAttach"), FGameplayEventData());
	Ability->Level = Level;
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, FGameplayTag::RequestGameplayTag("Event.TriggerPassiveDetach"), FGameplayEventData());
}
void AAP_Hero::ActivateAbility(int AbilitySlot)
{
	bool valid = AbilityHandles.IsValidIndex(AbilitySlot);
	UE_LOG_FAST(TEXT("about to activate ability %d, valid=%d"), AbilitySlot, valid);
	if (!valid || !AbilitySystem)
	{
		return;
	}
	// If bAllowRemoteActivation is true, it will remotely activate local / server abilities, if false it will only try to locally activate the ability
	bool ret = AbilitySystem->TryActivateAbility(AbilityHandles[AbilitySlot], true);
	if (ret)
	{
		AbilityStates[AbilitySlot] = EAbilityState::Casting;
		AbilityChangedDelegate.Broadcast(EAbilityState::Casting, AbilitySlot);
	}
	UE_LOG_FAST(TEXT("activate ability, ret = %d"), ret);
}

void AAP_Hero::CancelAllAttack()
{
	if (!AbilitySystem)
	{
		return;
	}
	AbilitySystem->CancelAllAbilities();
	UE_LOG_FAST(TEXT("cancel all ability"));
}

void AAP_Hero::RemoveAllChannelingEffect()
{
	if (!AbilitySystem)
	{
		return;
	}
	if (!ChannelEffectCount)
	{
		return;
	}
	static FGameplayTagContainer Container =
		FGameplayTag::RequestGameplayTag("Combat.Channeling").GetSingleTagContainer();
	AbilitySystem->RemoveActiveEffectsWithGrantedTags(Container);
	UE_LOG_FAST(TEXT("cancel all channeling ability"));
	ChannelEffectCount = 0;
}

float AAP_Hero::GetAbilityCooldown(int AbilitySlot)
{
	bool valid = AbilitySystem
		&& AbilityHandles.IsValidIndex(AbilitySlot)
		&& AbilitySystem->GetActivatableAbilities().IsValidIndex(AbilitySlot)
		&& AbilitySystem->GetActivatableAbilities()[AbilitySlot].GetAbilityInstances().Num();
	UE_LOG_FAST(TEXT("about to check ability cooldown %d, valid=%d"), AbilitySlot, valid);
	float ret = 0.0f;
	if (!valid)
	{
		UE_LOG_FAST(TEXT("valid false return %f"), ret);
		return ret;
	}
	UGameplayAbility* ability = AbilitySystem->GetActivatableAbilities()[AbilitySlot].GetAbilityInstances().Last();
	ret = ability->GetCooldownTimeRemaining();
	UE_LOG_FAST(TEXT("return %f"), ret);
	return ret;
}

float AAP_Hero::GetAbilityCooldownPercent(int AbilitySlot)
{
	bool valid = AbilitySystem 
		&& AbilityHandles.IsValidIndex(AbilitySlot)
		&& AbilitySystem->GetActivatableAbilities().IsValidIndex(AbilitySlot)
		&& AbilitySystem->GetActivatableAbilities()[AbilitySlot].GetAbilityInstances().Num();
	UE_LOG_FAST(TEXT("about to check ability cooldown percent %d, valid=%d"), AbilitySlot, valid);
	float ret = 0.0f;
	if (!valid)
	{
		UE_LOG_FAST(TEXT("valid false return %f"), ret);
		return ret;
	}
	UGameplayAbility* ability = AbilitySystem->GetActivatableAbilities()[AbilitySlot].GetAbilityInstances().Last();
	FGameplayAbilitySpecHandle handle = AbilityHandles[AbilitySlot];
	float remaining;
	float duration;
	ability->GetCooldownTimeRemainingAndDuration(handle, ability->GetCurrentActorInfo(), remaining, duration);
	if (duration == 0.0f)
	{
		UE_LOG_FAST(TEXT("duration = 0, return %f"), ret);
		return ret;
	}
	ret = remaining / duration;
	UE_LOG_FAST(TEXT("return %f"), ret);
	return ret;
}

EAbilityState AAP_Hero::GetAbilityState(int AbilitySlot)
{
	if (!AbilityStates.IsValidIndex(AbilitySlot))
	{
		return EAbilityState::Disabled;
	}
	return AbilityStates[AbilitySlot];
}

bool AAP_Hero::GetAbilityAutoCastEnabled(int AbilitySlot)
{
	bool valid = AbilitySystem
		&& AbilitySystem->GetActivatableAbilities().IsValidIndex(AbilitySlot);
	if (!valid)
	{
		return false;
	}
	UGameplayAbility* Ability = AbilitySystem->GetActivatableAbilities()[AbilitySlot].Ability;
	bool implemented = Ability->AbilityTags.HasAny(
		FGameplayTag::RequestGameplayTag("Combat.Autocast").GetSingleTagContainer());
	bool enabled = true;
	return implemented && enabled;
}

bool AAP_Hero::IsAbilityBusy(int AbilitySlot)
{
	bool valid = AbilitySystem
		&& AbilityHandles.IsValidIndex(AbilitySlot);
	if (!valid)
	{
		return false;
	}
	auto Ability = AbilitySystem->FindAbilitySpecFromHandle(AbilityHandles[AbilitySlot]);
	return Ability->IsActive();
}

// Called every frame
void AAP_Hero::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (!GetCharacterMovement()->Velocity.IsNearlyZero())
	{
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
	if (!AbilitySystem)
	{
		return;
	}
	AbilitySystem->RefreshAbilityActorInfo();
}

void AAP_Hero::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AAP_Hero, GameplayTags);
	DOREPLIFETIME(AAP_Hero, LogicalController);
}

UAbilitySystemComponent * AAP_Hero::GetAbilitySystemComponent() const
{
	return AbilitySystem;
}

void AAP_Hero::SetGenericTeamId(const FGenericTeamId& Id)
{
	if (TeamId == Id)
	{
		return;
	}
	auto T1 = FGameplayTag::RequestGameplayTag("Combat.Team_1");
	auto T2 = FGameplayTag::RequestGameplayTag("Combat.Team_2");
	auto TN = FGameplayTag::RequestGameplayTag("Combat.Team_Neutral");
	RemoveGameplayTag(TeamId == (uint8)EGameTeam::Team1 ? T1 :
		TeamId == (uint8)EGameTeam::Team2 ? T2 :
		TN);
	AddGameplayTag(Id == (uint8)EGameTeam::Team1 ? T1 :
		Id == (uint8)EGameTeam::Team2 ? T2 :
		TN);
	TeamId = Id;
	UE_LOG_FAST(TEXT("AAP_Hero::SetGenericTeamId %d"), (uint8)TeamId);
	OnTeamUpdated(Id);
}

FGenericTeamId AAP_Hero::GetGenericTeamId() const
{
	UE_LOG_FAST(TEXT("AAP_Hero::GetGenericTeamId %d"), (uint8)TeamId);
	return TeamId;
}

void AAP_Hero::OnGameplayEffectAppliedToSelf(UAbilitySystemComponent * Source, const FGameplayEffectSpec & Spec, FActiveGameplayEffectHandle Handle)
{
	UE_LOG_FAST(TEXT("AAP_Hero::OnGameplayEffectAppliedToSelf duration = %f"), Spec.GetDuration());
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
	UE_LOG_FAST(TEXT("AAP_Hero::OnAbilityActivated %s"), *Ability->GetFName().ToString());
	FGameplayAbilitySpecHandle Handle = Ability->GetCurrentAbilitySpecHandle();
	int Index = AbilityHandles.Find(Handle);
	if (Index == INDEX_NONE)
	{
		return;
	}
	AbilityStates[Index] = EAbilityState::Casting;
	AbilityChangedDelegate.Broadcast(EAbilityState::Casting, Index);
}

void AAP_Hero::OnAbilityCommitted(UGameplayAbility* Ability)
{
	UE_LOG_FAST(TEXT("AAP_Hero::OnAbilityCommitted %s"), *Ability->GetFName().ToString());
	FGameplayAbilitySpecHandle Handle = Ability->GetCurrentAbilitySpecHandle();
	int Index = AbilityHandles.Find(Handle);
	if (Index == INDEX_NONE)
	{
		return;
	}
	if (Ability->CheckCooldown(Handle, Ability->GetCurrentActorInfo()))
	{
		return;
	}
	AbilityStates[Index] = EAbilityState::OnCooldown;
	AbilityChangedDelegate.Broadcast(EAbilityState::OnCooldown, Index);
	FGameplayEffectQuery query;
	query.EffectDefinition = Ability->GetCooldownGameplayEffect()->GetClass();
	TArray<FActiveGameplayEffectHandle> Handles = AbilitySystem->GetActiveEffects(query);
	UE_LOG_FAST(TEXT("setting up delegate..."));
	if (!Handles.Num())
	{
		UE_LOG_FAST(TEXT("can't find cooldown effect, setup unsuccessful"));
		AbilityStates[Index] = EAbilityState::Ready;
		AbilityChangedDelegate.Broadcast(EAbilityState::Ready, Index);
		return;
	}
	FActiveGameplayEffectHandle CooldownEffect = Handles.Last();
	FOnActiveGameplayEffectRemoved_Info* Delegate = AbilitySystem->OnGameplayEffectRemoved_InfoDelegate(CooldownEffect);
	if (!Delegate)
	{
		UE_LOG_FAST(TEXT("can't find delegate, setup unsuccessful"));
		return;
	}
	Delegate->AddUObject(this, &AAP_Hero::OnAbilityOffCooldown);
	UE_LOG_FAST(TEXT("delegate setup successful"));
}

void AAP_Hero::OnAbilityEnded(UGameplayAbility* Ability)
{
	UE_LOG_FAST(TEXT("AAP_Hero::OnAbilityEnded %s"), *Ability->GetFName().ToString());
	FGameplayAbilitySpecHandle Handle = Ability->GetCurrentAbilitySpecHandle();
	int Index = AbilityHandles.Find(Handle);
	if (Index == INDEX_NONE)
	{
		return;
	}
	if (Ability->CheckCooldown(Handle, Ability->GetCurrentActorInfo()))
	{
		AbilityStates[Index] = EAbilityState::OnCooldown;
		//AbilityChangedDelegate.Broadcast(EAbilityState::OnCooldown, Index);
		return;
	}
	AbilityStates[Index] = EAbilityState::Ready;
	AbilityChangedDelegate.Broadcast(EAbilityState::Ready, Index);
}

void AAP_Hero::OnAbilityOffCooldown(const FGameplayEffectRemovalInfo& InGameplayEffectRemovalInfo)
{
	UE_LOG_FAST(TEXT("AAP_Hero::OnAbilityOffCooldown"));
	// this is not the actual ability we are concerning, just a default ability instance
	// weird but i don't know why
	const UGameplayAbility* AbilityInfo = InGameplayEffectRemovalInfo.EffectContext.GetAbility();
	int slot = -1;
	for (auto handle: AbilityHandles)
	{
		auto spec = AbilitySystem->FindAbilitySpecFromHandle(handle);
		if (!spec)
		{
			continue;
		}
		if (spec->Ability->GetClass() == AbilityInfo->GetClass())
		{
			slot = AbilityHandles.IndexOfByKey(handle);
			break;
		}
	}
	if (slot == -1)
	{
		UE_LOG_FAST(TEXT("an ability just gone off cooldown, but it seems doesn't belong to this actor %s/%s"), *GetFName().ToString(), *AbilityInfo->GetFName().ToString());
		return;
	}
	AbilityStates[slot] = EAbilityState::Ready;
	AbilityChangedDelegate.Broadcast(EAbilityState::Ready, slot);
}

void AAP_Hero::GetAllUnitInRange(TArray<AAP_Hero*>& Result, float Radius)
{
	const static ECollisionChannel Channel = ECollisionChannel::ECC_GameTraceChannel11;
	FCollisionQueryParams Params(SCENE_QUERY_STAT(SphereTargetingOverlap), false);
	TArray<FOverlapResult> Overlaps;
	FCollisionShape Shape = FCollisionShape::MakeSphere(Radius);
	GetWorld()->OverlapMultiByChannel(Overlaps, GetActorLocation(), FQuat::Identity, Channel, Shape, Params);
#if ENABLE_DRAW_DEBUG
	DrawDebugSphere(GetWorld(), GetActorLocation(), Radius, 10, FColor::Green, false, 1.5f);
#endif
	for (auto Overlap: Overlaps)
	{
		if (!Overlap.bBlockingHit)
		{
			continue;
		}
		auto Unit = Cast<AAP_Hero>(Overlap.Actor);
		if (!Unit)
		{
			continue;
		}
		Result.AddUnique(Unit);
	}
	UE_LOG_FAST(TEXT("AAP_Hero::GetAllUnitInRange, at %s,  got %d"), *GetActorLocation().ToString(), Result.Num());
}

void AAP_Hero::GetAllEnemyInRange(TArray<AAP_Hero*>& Result, float Radius)
{
	GetAllUnitInRange(Result, Radius);
	for (int i = Result.Num() - 1; i >= 0; i--)
	{
		auto Unit = Result[i];
		if (GetTeamAttitudeTowards(*Unit) == ETeamAttitude::Hostile)
		{
			continue;
		}
		Result.RemoveAt(i);
	}
}

void AAP_Hero::GetAllAllyInRange(TArray<AAP_Hero*>& Result, float Radius)
{
	GetAllUnitInRange(Result, Radius);
	for (int i = Result.Num() - 1; i >= 0; i--)
	{
		auto Unit = Result[i];
		if (GetTeamAttitudeTowards(*Unit) == ETeamAttitude::Friendly)
		{
			continue;
		}
		Result.RemoveAt(i);
	}
}

void AAP_Hero::GrantBountyExp()
{
	TArray<AAP_Hero*> Enemies;
	GetAllEnemyInRange(Enemies, EXP_AQUIRING_RANGE);
	UE_LOG_FAST(TEXT("AAP_Hero::GrantBountyExp got %d enemies"), Enemies.Num());
	if (!Enemies.Num())
	{
		return;
	}
	// exp given per unit gets smaller as enemy count grows, but not linearly
	float ExpPerUnit = AllStats_->GetBountyExp() / FMath::Sqrt(Enemies.Num());
	for (int i = Enemies.Num() - 1; i >= 0; i--)
	{
		auto Enemy = Enemies[i];
		Enemy->GiveExp(ExpPerUnit);
	}
}

void AAP_Hero::OnStatsChanged(const FGameplayAttribute& Attribute, float NewValue)
{
	// at this moment, AllStats_->Get<attribute name here>() still return the old value, use with caution
	if (Attribute == UAP_AttributeSet::GetHealthAttribute())
	{
		return HandleHealthChanged(NewValue);
	}
	if (Attribute == UAP_AttributeSet::GetManaAttribute())
	{
		return HandleManaChanged(NewValue);
	}
	if (Attribute == UAP_AttributeSet::GetLevelAttribute())
	{
		return HandleLevelChanged(NewValue);
	}
	if (Attribute == UAP_AttributeSet::GetMaxHealthAttribute())
	{
		return HandleHealthChanged(NewValue);
	}
	if (Attribute == UAP_AttributeSet::GetExperienceAttribute())
	{
		return HandleExpChanged(NewValue);
	}
	if (Attribute == UAP_AttributeSet::GetMoveSpeedAttribute())
	{
		return HandleMoveSpeedChanged(NewValue);
	}
	if (Attribute == UAP_AttributeSet::GetTurnRateAttribute())
	{
		return HandleTurnRateChanged(NewValue);
	}
	StatChangedDelegate.Broadcast(Attribute, NewValue);
}

void AAP_Hero::HandleHealthChanged(float NewValue)
{
	UE_LOG_FAST(TEXT("AAP_Hero::HandleHealthChanged %f"), NewValue);
	if (!bStatsInitialized)
	{
		return;
	}
	float m = AllStats_->GetMaxHealth();
	OnHealthChanged(NewValue, NewValue / (m > 0.0f ? m : 1.0f));
	if (!HasAuthority())
	{
		return;
	}
	if (NewValue > 0.0f)
	{
		return;
	}
	GrantBountyExp();
	EnterDeath();
}

void AAP_Hero::HandleManaChanged(float NewValue)
{
	if (!bStatsInitialized)
	{
		return;
	}
	float m = AllStats_->GetMaxMana();
	OnManaChanged(NewValue, NewValue / (m > 0.0f ? m : 1.0f));
}
void AAP_Hero::HandleLevelChanged(float NewValue)
{
	if (!bStatsInitialized)
	{
		return;
	}
	OnLevelChanged(NewValue);
}
void AAP_Hero::HandleExpChanged(float NewValue)
{
	if (!bStatsInitialized)
	{
		return;
	}
	UE_LOG_FAST(TEXT("exp changed, exp now is %f/%f"), NewValue, AllStats_->GetRequiredExp());
	OnExpChanged(NewValue, GetExpPercent());
}

void AAP_Hero::HandleMoveSpeedChanged(float NewValue)
{
	UE_LOG_FAST(TEXT("AAP_Hero::HandleMoveSpeedChanged"));
	if (!bStatsInitialized)
	{
		return;
	}
	GetCharacterMovement()->MaxWalkSpeed = NewValue;
}

void AAP_Hero::HandleTurnRateChanged(float NewValue)
{
	UE_LOG_FAST(TEXT("AAP_Hero::HandleTurnRateChanged"));
	if (!bStatsInitialized)
	{
		return;
	}
	GetCharacterMovement()->RotationRate.Yaw = NewValue;
}

void AAP_Hero::HandleItemChanged()
{
	EWeaponCategory new_weapon = EWeaponCategory::BareHand;
	for(auto item : EquipedItems)
	{
		auto c = item->Category;
		new_weapon = c == EItemCategory::WeaponAxe ? EWeaponCategory::Axe :
			c == EItemCategory::WeaponBow ? EWeaponCategory::Bow :
			c == EItemCategory::WeaponDagger ? EWeaponCategory::Dagger :
			c == EItemCategory::WeaponDualSword ? EWeaponCategory::DualSword :
			c == EItemCategory::WeaponKatana ? EWeaponCategory::Katana1 :
			c == EItemCategory::WeaponMagicStaff ? EWeaponCategory::MagicStaff :
			c == EItemCategory::WeaponSwordAndShield ? EWeaponCategory::SwordAndShield :
			new_weapon;
		if (new_weapon != EWeaponCategory::BareHand)
		{
			break;
		}
	}
	if (new_weapon != CurrentWeapon_)
	{
		OnWeaponChanged(CurrentWeapon_, new_weapon);
	}
	OnEquipmentChanged();
}

void AAP_Hero::SelectHero(bool selected)
{
	IsSelected = selected;
	SelectionRing->SetVisibility(IsSelected);
}

float AAP_Hero::GetHealthPercent() const
{
	if (!AllStats_->IsValidLowLevel())
	{
		return 0.0f;
	}
	const float v = AllStats_->GetHealth();
	const float m = FMath::Max(1.0f, AllStats_->GetMaxHealth());
	return v / m;
}

float AAP_Hero::GetManaPercent() const
{
	if (!AllStats_->IsValidLowLevel())
	{
		return 0.0f;
	}
	const float v = AllStats_->GetMana();
	const float m = FMath::Max(1.0f, AllStats_->GetMaxMana());
	return v / m;
}

float AAP_Hero::GetExpPercent() const
{
	if (!AllStats_->IsValidLowLevel())
	{
		return 0.0f;
	}
	const float v = AllStats_->GetExperience();
	const float m = FMath::Max(1.0f, AllStats_->GetRequiredExp());
	return v / m;
}

int32 AAP_Hero::GetAbilityCount() const
{
	return AbilityHandles.Num();
}

UGameplayAbility* AAP_Hero::GetAbility(int AbilitySlot) const
{
	bool valid = AbilitySystem
		&& AbilitySystem->GetActivatableAbilities().IsValidIndex(AbilitySlot);
	if (!valid)
	{
		return nullptr;
	}
	auto Ability = AbilitySystem->GetActivatableAbilities()[AbilitySlot];
	return AbilitySystem->GetActivatableAbilities()[AbilitySlot].Ability;
}

bool AAP_Hero::CanActivateAbility(int AbilitySlot) const
{
	bool valid = AbilitySystem
		&& AbilitySystem->GetActivatableAbilities().IsValidIndex(AbilitySlot)
		&& AbilityHandles.IsValidIndex(AbilitySlot);
	if (!valid)
	{
		UE_LOG_FAST(TEXT("AAP_Hero::CanActivateAbility false because invalid index"));
		return false;
	}
	auto Ability = AbilitySystem->GetActivatableAbilities()[AbilitySlot].Ability;
	auto Handle = AbilityHandles[AbilitySlot];
	auto ActorInfo = AbilitySystem->AbilityActorInfo.Get();
	FGameplayTagContainer ASTags;
	AbilitySystem->GetOwnedGameplayTags(ASTags);
	return Ability->CanActivateAbility(Handle, ActorInfo, &ASTags);
}

bool AAP_Hero::LevelUpAbility(int AbilitySlot)
{
	bool valid = AbilitySystem
		&& AbilitySystem->GetActivatableAbilities().IsValidIndex(AbilitySlot)
		&& AbilityHandles.IsValidIndex(AbilitySlot);
	if (!valid)
	{
		return false;
	}
	FGameplayAbilitySpec& Ability = AbilitySystem->GetActivatableAbilities()[AbilitySlot];
	float Potential = AllStats_->GetAbilityPoint();
	if (Potential < 1.0f)
	{
		return false;
	}
	AllStats_->SetAbilityPoint(Potential - 1.0f);
	Ability.Level += 1;
	return true;
}

void AAP_Hero::EnterCloak_Implementation(ECloakingLevel Level)
{
	// invisible to AI and visual render, character still physically on the map
	switch (Level)
	{
	case ECloakingLevel::Cloaked:
		// invisible to enemy AI, manual ray tracing only, can still be visually seen
		break;
	case ECloakingLevel::Invisible:
		// invisible to enemy AI, manual ray tracing, visual, can still be logically queried by ability
		break;
	case ECloakingLevel::Vanished:
		// logically and visually disappeared on the map, for both enemy and ally,
		// all logic regarding unit's position will not guaranteed to work
		break;
	default:
		break;
	}
	CloakStatus = Level;
	OnCloakStarted(Level);
}

void AAP_Hero::QuitCloak_Implementation()
{
	auto LastStatus = CloakStatus;
	CloakStatus = ECloakingLevel::None;
	OnCloakFinished(LastStatus);
}

void AAP_Hero::Respawn_Implementation()
{
	AllStats_->SetHealth(AllStats_->GetMaxHealth());
	AllStats_->SetMana(AllStats_->GetMaxMana());
	SpawnDefaultController();
	OnRespawn();
}

void AAP_Hero::EnterDeath_Implementation()
{
	AbilitySystem->CancelAllAbilities();
	// TODO: remove all timed buff
	GetCharacterMovement()->StopMovementImmediately();
	float DeathTime = AllStats_->GetDeathTime();
	if (GetController() && GetController()->IsValidLowLevel())
	{
		auto AIController = Cast<AAIController>(GetController());
		AIController->UnPossess();
		AIController->Destroy();
	}
	else
	{
		UE_LOG_FAST(TEXT("found no controller, weird"));
	}
	OnDeath(DeathTime);
	FTimerHandle DeathTimerHandle;
	GetWorldTimerManager().SetTimer(DeathTimerHandle,
		this, &AAP_Hero::Respawn, DeathTime, false, -1.0f);
	SelectHero(false);
}

void AAP_Hero::GiveItem_Implementation(TSubclassOf<UAP_GameplayItem> Item)
{
	if (!HasAuthority())
	{
		return;
	}
	UAP_GameplayItem* item = Item.GetDefaultObject();
	StashItems.Add(item);
}

void AAP_Hero::EquipItem_Implementation(int Slot)
{
	if (!HasAuthority())
	{
		return;
	}
	if (!StashItems.IsValidIndex(Slot))
	{
		return;
	}
	UAP_GameplayItem* item = StashItems[Slot];
	if (EquipedItems.Num() > 6)
	{
		return;
	}
	EquipedItems.Add(item);
	for (auto Ability : item->GrantedAbilities)
	{
		GiveAbility(Ability);
	}
	HandleItemChanged();
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, FGameplayTag::RequestGameplayTag("Event.TriggerPassiveAttach"), FGameplayEventData());
}

void AAP_Hero::GiveEquipItem_Implementation(TSubclassOf<UAP_GameplayItem> Item)
{
	if (!HasAuthority())
	{
		return;
	}
	if (EquipedItems.Num() > 6)
	{
		return;
	}
	EquipedItems.Add(Item);
	for (auto Ability : Item->GrantedAbilities)
	{
		GiveAbility(Ability);
	}
	HandleItemChanged();
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, FGameplayTag::RequestGameplayTag("Event.TriggerPassiveAttach"), FGameplayEventData());
}

void AAP_Hero::UnequipItem_Implementation(int Slot)
{
	if (!HasAuthority())
	{
		return;
	}
	if (!EquipedItems.IsValidIndex(Slot))
	{
		return;
	}
	if (StashItems.Num() > 8)
	{
		return;
	}
	auto item = EquipedItems[Slot];
	StashItems.Add(item);
	for (auto Ability : item.GrantedAbilities)
	{
		RemoveAbility(Ability);
	}
	EquipedItems.RemoveAt(Slot);
	HandleItemChanged();
}

void AAP_Hero::RemoveItem_Implementation(int Slot)
{
	if (!HasAuthority())
	{
		return;
	}
	if (!StashItems.IsValidIndex(Slot))
	{
		return;
	}
	StashItems.RemoveAt(Slot);
}

void AAP_Hero::UnequipRemoveItem_Implementation(int Slot)
{
	if (!HasAuthority())
	{
		return;
	}
	if (!EquipedItems.IsValidIndex(Slot))
	{
		return;
	}
	EquipedItems.RemoveAt(Slot);
	HandleItemChanged();
}

bool AAP_Hero::IsVisibleToTeam(FGameplayTag TeamTag)
{
	if (GameplayTags.HasTag(TeamTag))
	{
		return CloakStatus != ECloakingLevel::Vanished;
	}
	return CloakStatus == ECloakingLevel::None;
}

bool AAP_Hero::RemoveGameplayTag(FGameplayTag Tag)
{
	return GameplayTags.RemoveTag(Tag);
}

void AAP_Hero::AddGameplayTag(FGameplayTag Tag)
{
	GameplayTags.AddTag(Tag);
}

void AAP_Hero::GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const
{
	TagContainer = GameplayTags;
}

bool AAP_Hero::HasMatchingGameplayTag(FGameplayTag TagToCheck) const
{
	return GameplayTags.HasTag(TagToCheck);
}

bool AAP_Hero::HasAllMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const
{
	return GameplayTags.HasAll(TagContainer);
}

bool AAP_Hero::HasAnyMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const
{
	return GameplayTags.HasAny(TagContainer);
}

bool AAP_Hero::IsLogicalController(AController* OtherController)
{
	return OtherController == LogicalController;
}

void AAP_Hero::SetLogicalController(AController* NewController)
{
	LogicalController = NewController;
	auto RTSController = Cast<AAP_RTSPlayerController>(NewController);
	if (!RTSController->IsValidLowLevel())
	{
		return;
	}
	SetTeam(RTSController->Team);
}

void AAP_Hero::SetTeam(const EGameTeam Id)
{
	SetGenericTeamId((uint8)Id);
}

EGameTeam AAP_Hero::GetTeam()
{
	return (EGameTeam)(uint8)GetGenericTeamId();
}

bool AAP_Hero::IsAlly()
{
	auto Me = Cast<AAP_RTSPlayerController>(GetWorld()->GetFirstPlayerController());
	auto ThisUnit = Cast<AAP_RTSPlayerController>(LogicalController);
	if (Me && ThisUnit)
	{
		return Me->GetAttituteTowardPlayer(ThisUnit) == ETeamAttitude::Type::Friendly;
	}
	return false;
}

bool AAP_Hero::IsHostile()
{
	auto Me = Cast<AAP_RTSPlayerController>(GetWorld()->GetFirstPlayerController());
	auto ThisUnit = Cast<AAP_RTSPlayerController>(LogicalController);
	if (Me && ThisUnit)
	{
		return Me->GetAttituteTowardPlayer(ThisUnit) == ETeamAttitude::Type::Hostile;
	}
	return false;
}

bool AAP_Hero::IsDead() const
{
	return AllStats_->GetHealth() <= 0.0f;
}
