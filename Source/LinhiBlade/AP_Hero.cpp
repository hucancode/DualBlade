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

	AllStats = CreateDefaultSubobject<UAP_AttributeSet>(TEXT("AllStats"));;
	UE_LOG(LogTemp, Warning, TEXT("AAP_Hero::AAP_Hero(), AllStats=%x"), AllStats);

	AbilitySet = 0;
	bStatsInitialized = false;
	bAbilitiesInitialized = false;
	ChannelEffectCount = 0;
	SetReplicates(true);
}

// Called when the game starts or when spawned
void AAP_Hero::BeginPlay()
{
	Super::BeginPlay();
	//UE_LOG(LogTemp, Warning, TEXT("AAP_Hero::BeginPlay(), AllStats=%x"), AllStats);
	SetupAbilities();
	SetupStats();
	AbilitySystem->AbilityCommittedCallbacks.AddUObject(this, &AAP_Hero::OnAbilityCommitted);
	AbilitySystem->AbilityEndedCallbacks.AddUObject(this, &AAP_Hero::OnAbilityEnded);
	AbilitySystem->AbilityActivatedCallbacks.AddUObject(this, &AAP_Hero::OnAbilityActivated);
	if (!GetController()->IsValidLowLevel())
	{
		SpawnDefaultController();
	}
	if (HasMatchingGameplayTag(
		FGameplayTag::RequestGameplayTag("Combat.Team_1")))
	{
		SetGenericTeamId((uint8)EGameTeam::Team1);
	}
	else if (HasMatchingGameplayTag(
		FGameplayTag::RequestGameplayTag("Combat.Team_2")))
	{
		SetGenericTeamId((uint8)EGameTeam::Team2);
	}
	else
	{
		SetGenericTeamId((uint8)EGameTeam::Neutral);
	}
}

void AAP_Hero::SetupAbilities()
{
	if (Role == ROLE_Authority && !bAbilitiesInitialized)
	{
		const int32 Level = 0;
		if (!AbilitySet)
		{
			return;
		}
		GiveAbility(Level, AbilitySet->Find(EAbilitySlot::Ability1));
		GiveAbility(Level, AbilitySet->Find(EAbilitySlot::Ability2));
		GiveAbility(Level, AbilitySet->Find(EAbilitySlot::Ability3));
		GiveAbility(Level, AbilitySet->Find(EAbilitySlot::Ability4));
		GiveAbility(Level, AbilitySet->Find(
			Job == EJob::Job1 ? EAbilitySlot::Ability5 :
			Job == EJob::Job2 ? EAbilitySlot::Ability6 :
			Job == EJob::Job3 ? EAbilitySlot::Ability7 :
			Job == EJob::Job4 ? EAbilitySlot::Ability8 :
			Job == EJob::Job5 ? EAbilitySlot::Ability9 : 
			EAbilitySlot::Ability5));
		GiveAbility(Level, AbilitySet->Find(EAbilitySlot::Ability10));
		GiveAbility(Level, AbilitySet->Find(EAbilitySlot::Ability11));
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, FGameplayTag::RequestGameplayTag("Event.TriggerPassiveAttach"), FGameplayEventData());
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

void AAP_Hero::GiveExp(float Amount)
{
	UE_LOG(LogTemp, Warning, TEXT("AAP_Hero::GiveExp %f"), Amount);
	AllStats->SetExperience(AllStats->GetExperience() + Amount);
}

void AAP_Hero::SetupStats()
{
	if (Role == ROLE_Authority && !bStatsInitialized)
	{
		/*UE_LOG(LogTemp, Warning, TEXT("setup stats, starting..."));*/
		FGameplayEffectContextHandle EffectContext = AbilitySystem->MakeEffectContext();
		EffectContext.AddSourceObject(this);

		FGameplayEffectSpecHandle NewHandle = AbilitySystem->MakeOutgoingSpec(BaseStats, 1.0f, EffectContext);
		if (NewHandle.IsValid())
		{
			FActiveGameplayEffectHandle ActiveGEHandle = AbilitySystem->ApplyGameplayEffectSpecToTarget(*NewHandle.Data.Get(), AbilitySystem);
			bStatsInitialized = true;
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("won't setup stats because this is not server"));
	}
}

void AAP_Hero::GiveAbility(const int32& Level, TSubclassOf<UGameplayAbility> Ability)
{
	if (!Ability->IsValidLowLevel())
	{
		UE_LOG(LogTemp, Warning, TEXT("AAP_Hero::GiveAbility failed"));
		return;
	}
	FGameplayAbilitySpecHandle handle = AbilitySystem->GiveAbility(
		FGameplayAbilitySpec(Ability, Level, INDEX_NONE, this));
	AbilityHandles.Add(handle);
	AbilityStates.AddDefaulted();
}

void AAP_Hero::RemoveAllAbilities()
{
	if (Role == ROLE_Authority)
	{
		AbilityHandles.Empty();
		AbilityStates.Empty();
		AbilitySystem->CancelAllAbilities();
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, 
			FGameplayTag::RequestGameplayTag("Event.TriggerPassiveDetach"), FGameplayEventData());
		const auto AllAbility = AbilitySystem->GetActivatableAbilities();
		for (auto Ability : AllAbility)
		{
			AllStats->SetAbilityPoint(AllStats->GetAbilityPoint() + Ability.Level);
		}
		AbilityPointChange.Broadcast();
		// potential crash here (known bug, but still have't found any effective fix)
		// basically you need to make sure no skill are in the middle of activation before remove them
		AbilitySystem->ClearAllAbilities();
		bAbilitiesInitialized = false;
	}
}

int AAP_Hero::GetAbilityLevel(int AbilitySlot)
{
	bool valid = AbilityHandles.IsValidIndex(AbilitySlot);
	if (valid && AbilitySystem && bAbilitiesInitialized)
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
	UE_LOG(LogTemp, Warning, TEXT("about to set ability level %d, valid=%d"), AbilitySlot, valid);
	if (valid && AbilitySystem && bAbilitiesInitialized)
	{
		auto Ability = AbilitySystem->FindAbilitySpecFromHandle(AbilityHandles[AbilitySlot]);
		if (Ability)
		{
			UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, FGameplayTag::RequestGameplayTag("Event.TriggerPassiveAttach"), FGameplayEventData());
			Ability->Level = Level;
			UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, FGameplayTag::RequestGameplayTag("Event.TriggerPassiveDetach"), FGameplayEventData());
		}
	}
}
void AAP_Hero::ActivateAbility(int AbilitySlot)
{
	bool valid = AbilityHandles.IsValidIndex(AbilitySlot);
	UE_LOG(LogTemp, Warning, TEXT("about to activate ability %d, valid=%d"), AbilitySlot, valid);
	if (valid && AbilitySystem && bStatsInitialized && bAbilitiesInitialized)
	{
		// If bAllowRemoteActivation is true, it will remotely activate local / server abilities, if false it will only try to locally activate the ability
		bool ret = AbilitySystem->TryActivateAbility(AbilityHandles[AbilitySlot], true);
		if (ret)
		{
			AbilityStates[AbilitySlot] = EAbilityState::Casting;
			AbilityCastDelegate.Broadcast(AbilitySlot);
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

float AAP_Hero::GetAbilityCooldown(int AbilitySlot)
{
	bool valid = AbilitySystem
		&& AbilityHandles.IsValidIndex(AbilitySlot)
		&& AbilitySystem->GetActivatableAbilities().IsValidIndex(AbilitySlot)
		&& AbilitySystem->GetActivatableAbilities()[AbilitySlot].GetAbilityInstances().Num();
	UE_LOG(LogTemp, Warning, TEXT("about to check ability cooldown %d, valid=%d"), AbilitySlot, valid);
	float ret = 0.0f;
	if (!valid)
	{
		UE_LOG(LogTemp, Warning, TEXT("valid false return %f"), ret);
		return ret;
	}
	UGameplayAbility* ability = AbilitySystem->GetActivatableAbilities()[AbilitySlot].GetAbilityInstances().Last();
	ret = ability->GetCooldownTimeRemaining();
	UE_LOG(LogTemp, Warning, TEXT("return %f"), ret);
	return ret;
}

float AAP_Hero::GetAbilityCooldownPercent(int AbilitySlot)
{
	bool valid = AbilitySystem 
		&& AbilityHandles.IsValidIndex(AbilitySlot)
		&& AbilitySystem->GetActivatableAbilities().IsValidIndex(AbilitySlot)
		&& AbilitySystem->GetActivatableAbilities()[AbilitySlot].GetAbilityInstances().Num();
	UE_LOG(LogTemp, Warning, TEXT("about to check ability cooldown percent %d, valid=%d"), AbilitySlot, valid);
	float ret = 0.0f;
	if (!valid)
	{
		UE_LOG(LogTemp, Warning, TEXT("valid false return %f"), ret);
		return ret;
	}
	UGameplayAbility* ability = AbilitySystem->GetActivatableAbilities()[AbilitySlot].GetAbilityInstances().Last();
	FGameplayAbilitySpecHandle handle = AbilityHandles[AbilitySlot];
	float remaining;
	float duration;
	ability->GetCooldownTimeRemainingAndDuration(handle, ability->GetCurrentActorInfo(), remaining, duration);
	if (duration == 0.0f)
	{
		UE_LOG(LogTemp, Warning, TEXT("duration = 0, return %f"), ret);
		return ret;
	}
	ret = remaining / duration;
	UE_LOG(LogTemp, Warning, TEXT("return %f"), ret);
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
	if (AbilitySystem)
	{
		AbilitySystem->RefreshAbilityActorInfo();
	}
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
	UE_LOG(LogTemp, Warning, TEXT("AAP_Hero::SetGenericTeamId %d"), (uint8)TeamId);
	OnTeamUpdated(Id);
}

FGenericTeamId AAP_Hero::GetGenericTeamId() const
{
	UE_LOG(LogTemp, Warning, TEXT("AAP_Hero::GetGenericTeamId %d"), (uint8)TeamId);
	return TeamId;
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
	int Index = AbilityHandles.Find(Handle);
	if (Index == INDEX_NONE)
	{
		return;
	}
	AbilityStates[Index] = EAbilityState::Casting;
	AbilityCastDelegate.Broadcast(Index);
}

void AAP_Hero::OnAbilityCommitted(UGameplayAbility* Ability)
{
	UE_LOG(LogTemp, Warning, TEXT("AAP_Hero::OnAbilityCommitted %s"), *Ability->GetFName().ToString());
	FGameplayAbilitySpecHandle Handle = Ability->GetCurrentAbilitySpecHandle();
	int Index = AbilityHandles.Find(Handle);
	if (Index == INDEX_NONE)
	{
		return;
	}
	if (!Ability->CheckCooldown(Handle, Ability->GetCurrentActorInfo()))
	{
		AbilityStates[Index] = EAbilityState::OnCooldown;
		AbilityGoneCooldown.Broadcast(Index);
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
	int Index = AbilityHandles.Find(Handle);
	if (Index == INDEX_NONE)
	{
		return;
	}
	if (!Ability->CheckCooldown(Handle, Ability->GetCurrentActorInfo()))
	{
		AbilityStates[Index] = EAbilityState::OnCooldown;
		//AbilityGoneCooldown.Broadcast(Index);
	}
	else
	{
		AbilityStates[Index] = EAbilityState::Ready;
		AbilityOffCooldown.Broadcast(Index);
	}
}

void AAP_Hero::OnAbilityOffCooldown(const FGameplayEffectRemovalInfo& InGameplayEffectRemovalInfo)
{
	UE_LOG(LogTemp, Warning, TEXT("AAP_Hero::OnAbilityOffCooldown"));
	// this is not the actual ability we are concerning, just a default ability instance
	// weird but i don't know why
	const UGameplayAbility* AbilityInfo = InGameplayEffectRemovalInfo.EffectContext.GetAbility();
	EAbilitySlot Slot = AbilitySet->Find(AbilityInfo);
	if (Slot == EAbilitySlot::Ability_Invalid)
	{
		UE_LOG(LogTemp, Warning, TEXT("an ability just gone off cooldown, but it seems doesn't belong to this actor %s/%s"), *GetFName().ToString(), *AbilityInfo->GetFName().ToString());
		return;
	}
	AbilityStates[(int)Slot] = EAbilityState::Ready;
	AbilityOffCooldown.Broadcast((int)Slot);
}

void AAP_Hero::GetAllUnitInRange(TArray<AAP_Hero*>& Result, float Radius)
{
	const static ECollisionChannel Channel = ECollisionChannel::ECC_GameTraceChannel11;
	FCollisionQueryParams Params(SCENE_QUERY_STAT(SphereTargetingOverlap), false);
	TArray<FOverlapResult> Overlaps;
	FCollisionShape Shape = FCollisionShape::MakeSphere(Radius);
	UE_LOG(LogTemp, Warning, TEXT("AAP_Hero::GetAllUnitInRange %s"), *GetActorLocation().ToString());
	GetWorld()->OverlapMultiByChannel(Overlaps, GetActorLocation(), FQuat::Identity, Channel, Shape, Params);
#if ENABLE_DRAW_DEBUG
	DrawDebugSphere(GetWorld(), GetActorLocation(), Radius, 10, FColor::Green, false, 1.5f);
#endif
	for (auto Overlap: Overlaps)
	{
		if (Overlap.bBlockingHit)
		{
			auto Hero = Cast<AAP_Hero>(Overlap.Actor);
			if (Hero)
			{
				UE_LOG(LogTemp, Warning, TEXT("AAP_Hero::GetAllUnitInRange add %s"), *Hero->GetName());
				Result.Add(Hero);
			}
		}
	}
	UE_LOG(LogTemp, Warning, TEXT("AAP_Hero::GetAllUnitInRange got %d"), Result.Num());
}

void AAP_Hero::HandleHealthChanged(float NewValue)
{
	UE_LOG(LogTemp, Warning, TEXT("AAP_Hero::HandleHealthChanged %f"), NewValue);
	if (!bStatsInitialized)
	{
		return;
	}
	OnHealthChanged(NewValue);
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

void AAP_Hero::GrantBountyExp()
{
	TArray<AAP_Hero*> Enemies;
	GetAllUnitInRange(Enemies, EXP_AQUIRING_RANGE);
	for (int i = Enemies.Num() - 1; i >= 0; i--)
	{
		auto Enemy = Enemies[i];
		if (GetTeamAttitudeTowards(*Enemy) != ETeamAttitude::Hostile)
		{
			Enemies.RemoveAt(i);
		}
	}
	UE_LOG(LogTemp, Warning, TEXT("AAP_Hero::GrantBountyExp got %d enemies"), Enemies.Num());
	if (!Enemies.Num())
	{
		return;
	}
	// exp given per unit gets smaller as enemy count grows, but not linearly
	float ExpPerUnit = AllStats->GetBountyExp() / FMath::Sqrt(Enemies.Num());
	for (int i = Enemies.Num() - 1; i >= 0; i--)
	{
		auto Enemy = Enemies[i];
		Enemy->GiveExp(ExpPerUnit);
	}
}

void AAP_Hero::HandleManaChanged(float NewValue)
{
	if (!bStatsInitialized)
	{
		return;
	}
	OnManaChanged(NewValue);
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
	UE_LOG(LogTemp, Warning, TEXT("exp changed, exp now is %f/%f"), NewValue, AllStats->GetRequiredExp());
	OnExpChanged(NewValue);
}

void AAP_Hero::HandleMoveSpeedChanged(float NewValue)
{
	UE_LOG(LogTemp, Warning, TEXT("AAP_Hero::HandleMoveSpeedChanged"));
	if (!bStatsInitialized)
	{
		return;
	}
	GetCharacterMovement()->MaxWalkSpeed = NewValue;
}

void AAP_Hero::HandleTurnRateChanged(float NewValue)
{
	UE_LOG(LogTemp, Warning, TEXT("AAP_Hero::HandleTurnRateChanged"));
	if (!bStatsInitialized)
	{
		return;
	}
	GetCharacterMovement()->RotationRate.Yaw = NewValue;
}

void AAP_Hero::SelectHero(bool selected)
{
	IsSelected = selected;
	SelectionRing->SetVisibility(IsSelected);
}

float AAP_Hero::GetHealthPercent() const
{
	if (!AllStats->IsValidLowLevel())
	{
		return 0.0f;
	}
	const float v = AllStats->GetHealth();
	const float m = FMath::Max(1.0f, AllStats->GetMaxHealth());
	return v / m;
}

float AAP_Hero::GetManaPercent() const
{
	if (!AllStats->IsValidLowLevel())
	{
		return 0.0f;
	}
	const float v = AllStats->GetMana();
	const float m = FMath::Max(1.0f, AllStats->GetMaxMana());
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
		UE_LOG(LogTemp, Warning, TEXT("AAP_Hero::CanActivateAbility false because invalid index"));
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
	float Potential = AllStats->GetAbilityPoint();
	if (Potential < 1.0f)
	{
		return false;
	}
	AllStats->SetAbilityPoint(Potential - 1.0f);
	Ability.Level += 1;
	AbilityPointChange.Broadcast();
	AbilityLevelUp.Broadcast(AbilitySlot);
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
	AllStats->SetHealth(AllStats->GetMaxHealth());
	AllStats->SetMana(AllStats->GetMaxMana());
	OnRespawn();
}

void AAP_Hero::EnterDeath_Implementation()
{
	AbilitySystem->CancelAllAbilities();
	GetCharacterMovement()->StopMovementImmediately();
	float DeathTime = AllStats->GetDeathTime();
	OnDeath(DeathTime);
	FTimerHandle DeathTimerHandle;
	GetWorldTimerManager().SetTimer(DeathTimerHandle,
		this, &AAP_Hero::Respawn, DeathTime, false, -1.0f);
	SelectHero(false);
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
	if (RTSController->IsValidLowLevel())
	{
		SetTeam(RTSController->Team);
	}
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

void AAP_Hero::SetJob(EJob NewJob)
{
	// TODO: implement a job changing mechanic
	/*const int32 Level = 0;
	GiveAbility(Level, AbilitySet->Find(
		Job == EJob::Job1 ? EAbilitySlot::Ability5 :
		Job == EJob::Job2 ? EAbilitySlot::Ability6 :
		Job == EJob::Job3 ? EAbilitySlot::Ability7 :
		Job == EJob::Job4 ? EAbilitySlot::Ability8 :
		Job == EJob::Job5 ? EAbilitySlot::Ability9 :
		EAbilitySlot::Ability5));
	const auto AllAbility = AbilitySystem->GetActivatableAbilities();
	for (auto Ability : AllAbility)
	{
		AllStats->AbilityPoint.SetBaseValue(
			AllStats->AbilityPoint.GetBaseValue()
			+ Ability.Level);
	}*/
}
