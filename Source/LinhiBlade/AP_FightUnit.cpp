// Fill out your copyright notice in the Description page of Project Settings.


#include "AP_FightUnit.h"
#include "AIController.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"
#include "DrawDebugHelpers.h"
#include "AP_AttributeChangeData.h"
#include "Kismet/GameplayStatics.h"
#include "AP_RPGPlayerController.h"

#define ABILITY_MAX_LEVEL 4
#ifndef RAY_LENGTH
#define RAY_LENGTH 999999.0f
#endif

// Sets default values
AAP_FightUnit::AAP_FightUnit()
{
	AbilitySystem = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystem"));
	AbilityUser = CreateDefaultSubobject<UAbilityUser>(TEXT("AbilityUser"));
	ItemUser = CreateDefaultSubobject<UItemUser>(TEXT("ItemUser"));
	TeamAgent = CreateDefaultSubobject<UTeamAgent>(TEXT("TeamAgent"));
	Stats = CreateDefaultSubobject<UAP_AttributeSet>(TEXT("Stats"));

	SetReplicates(true);
	AbilitySystem->SetIsReplicated(true);
	AbilityUser->SetIsReplicated(true);
	ItemUser->SetIsReplicated(true);
	TeamAgent->SetIsReplicated(true);

	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
	GetCapsuleComponent()->InitCapsuleSize(5.f, 90.0f);
	//GetCharacterMovement()->bOrientRotationToMovement = true; // Rotate character to moving direction
	GetCharacterMovement()->bUseControllerDesiredRotation = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 640.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;
	GetCharacterMovement()->SetUpdateNavAgentWithOwnersCollisions(false);
	GetCharacterMovement()->NavAgentProps.AgentHeight = 90.0f;
	GetCharacterMovement()->NavAgentProps.AgentRadius = 5.0f;
	GetMesh()->SetRelativeRotation(FQuat::MakeFromEuler(FVector::DownVector * 90.0f));
	GetMesh()->SetRelativeLocation(FVector::DownVector * 85.0f);
	GetMesh()->bPerBoneMotionBlur = false;
	bUseControllerRotationPitch = 0;
	bUseControllerRotationYaw = 0;
	bUseControllerRotationRoll = 0;
}

// Called when the game starts or when spawned
void AAP_FightUnit::BeginPlay()
{
	Super::BeginPlay();
	if (GetGenericTeamId() == FGenericTeamId::NoTeam)
	{
		SetGenericTeamId((uint8)EGameTeam::Neutral);
	}
	FightStyle = EFightStyle::BareHand;
	AbilityUser->OnAbilityStateChanged.AddDynamic(this, &AAP_FightUnit::HandleAbilityStateChanged);
	AbilityUser->OnAbilitySlotChanged.AddDynamic(this, &AAP_FightUnit::HandleAbilitySlotChanged);
	AbilityUser->OnEffectApplied.AddDynamic(this, &AAP_FightUnit::HandleEffectApplied);
	AbilityUser->OnEffectRemoved.AddDynamic(this, &AAP_FightUnit::HandleEffectRemoved);
	Stats->OnAttributeChanged.AddDynamic(this, &AAP_FightUnit::HandleAttributeChanged);
	Stats->OnDeath.AddDynamic(this, &AAP_FightUnit::HandleDeath);
	Stats->OnLevelUp.AddDynamic(this, &AAP_FightUnit::HandleLevelUp);
	ItemUser->OnAbilityAdded.AddDynamic(this, &AAP_FightUnit::HandleAbilityAdded);
	ItemUser->OnAbilityRemoved.AddDynamic(this, &AAP_FightUnit::HandleAbilityRemoved);
	ItemUser->OnWeaponChanged.AddDynamic(this, &AAP_FightUnit::HandleWeaponChanged);
	TeamAgent->OnTeamChanged.AddDynamic(this, &AAP_FightUnit::HandleTeamChanged);
	TeamAgent->OnCloakStarted.AddDynamic(this, &AAP_FightUnit::HandleCloakStarted);
	TeamAgent->OnCloakFinished.AddDynamic(this, &AAP_FightUnit::HandleCloakFinished);
	if (HasAuthority())
	{
		Stats->InitFromMetaDataTable(StartingStats);
		ItemUser->GiveWeaponAbility(nullptr);
	}
}

// Called every frame
void AAP_FightUnit::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AAP_FightUnit::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AAP_FightUnit::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	AbilitySystem->RefreshAbilityActorInfo();
}

void AAP_FightUnit::OnRep_Controller()
{
	Super::OnRep_Controller();
	if (!AbilitySystem)
	{
		return;
	}
	AbilitySystem->RefreshAbilityActorInfo();
}

void AAP_FightUnit::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AAP_FightUnit, GameplayTags);
	//DOREPLIFETIME(AAP_FightUnit, LogicalController);
}

UAbilitySystemComponent* AAP_FightUnit::GetAbilitySystemComponent() const
{
	return AbilitySystem;
}

void AAP_FightUnit::SetGenericTeamId(const FGenericTeamId& Id)
{
	TeamAgent->SetTeam(static_cast<EGameTeam>((uint8)Id));
	UE_LOG_FAST(TEXT("AAP_Hero::SetGenericTeamId %d"), (uint8)Id);
}

FGenericTeamId AAP_FightUnit::GetGenericTeamId() const
{
	return (uint8)TeamAgent->GetTeam();
}

void AAP_FightUnit::GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const
{
	TagContainer = GameplayTags;
}

bool AAP_FightUnit::HasMatchingGameplayTag(FGameplayTag TagToCheck) const
{
	return GameplayTags.HasTag(TagToCheck);
}

bool AAP_FightUnit::HasAllMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const
{
	return GameplayTags.HasAll(TagContainer);
}

bool AAP_FightUnit::HasAnyMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const
{
	return GameplayTags.HasAny(TagContainer);
}

void AAP_FightUnit::HandleAbilityStateChanged()
{
}

void AAP_FightUnit::HandleAbilitySlotChanged()
{
}

void AAP_FightUnit::HandleEffectApplied(UAbilitySystemComponent* Source, const FGameplayEffectSpec& Spec, FActiveGameplayEffectHandle Handle)
{
}

void AAP_FightUnit::HandleEffectRemoved(const FGameplayEffectRemovalInfo& Info)
{
}

void AAP_FightUnit::HandleAttributeChanged(const FGameplayAttribute Attribute, float NewValue)
{
	if (Attribute == UAP_AttributeSet::GetMoveSpeedAttribute())
	{
		GetCharacterMovement()->MaxWalkSpeed = NewValue;
	}
	if (Attribute == UAP_AttributeSet::GetTurnRateAttribute())
	{
		GetCharacterMovement()->RotationRate.Yaw = NewValue;
	}
	auto tag = FGameplayTag::RequestGameplayTag("Combat.Trigger_Ability_On_Attribute_Changed");
	auto dto = NewObject<UAP_AttributeChangeData>(this, TEXT("DTO"));
	dto->NewValue = NewValue;
	dto->Attribute = Attribute;
	dto->AllAttribute = Stats;
	FGameplayEventData payload;
	payload.Instigator = this;
	payload.Target = this;
	payload.OptionalObject = dto;
	payload.EventTag = tag;
	if (AbilitySystem)
	{
		FScopedPredictionWindow NewScopedWindow(AbilitySystem, true);
		AbilitySystem->HandleGameplayEvent(tag, &payload);
	}
	dto->MarkPendingKill();
}

void AAP_FightUnit::HandleAbilityAdded(TSubclassOf<UAP_AbilityBase> Ability)
{
	AbilityUser->GiveAbility(Ability);
}

void AAP_FightUnit::HandleAbilityRemoved(TSubclassOf<UAP_AbilityBase> Ability)
{
	AbilityUser->RemoveAbility(Ability);
}

void AAP_FightUnit::HandleWeaponChanged()
{
	auto new_style = ItemUser->CurrentWeapon == EWeaponCategory::Axe ? EFightStyle::Axe :
		ItemUser->CurrentWeapon == EWeaponCategory::Bow ? EFightStyle::Bow :
		ItemUser->CurrentWeapon == EWeaponCategory::Dagger ? EFightStyle::Dagger :
		ItemUser->CurrentWeapon == EWeaponCategory::DualSword ? EFightStyle::DualSword :
		ItemUser->CurrentWeapon == EWeaponCategory::Katana ? EFightStyle::Katana1 :
		ItemUser->CurrentWeapon == EWeaponCategory::MagicStaff ? EFightStyle::MagicStaff :
		ItemUser->CurrentWeapon == EWeaponCategory::SwordAndShield ? EFightStyle::SwordAndShield :
		EFightStyle::BareHand;
	if (new_style != FightStyle)
	{
		FightStyle = new_style;
		OnFightStyleChanged.Broadcast();
	}
}

void AAP_FightUnit::HandleTeamChanged(EGameTeam Team)
{
	auto T1 = FGameplayTag::RequestGameplayTag("Combat.Team_1");
	auto T2 = FGameplayTag::RequestGameplayTag("Combat.Team_2");
	auto TN = FGameplayTag::RequestGameplayTag("Combat.Team_Neutral");
	GameplayTags.RemoveTag(T1);
	GameplayTags.RemoveTag(T2);
	GameplayTags.RemoveTag(TN);
	GameplayTags.AddTag(Team == EGameTeam::Team1 ? T1 : Team == EGameTeam::Team2 ? T2 : TN);
}

void AAP_FightUnit::HandleCloakStarted(ECloakingLevel Level)
{
	switch (Level)
	{
	case ECloakingLevel::Cloaked:
		break;
	case ECloakingLevel::Invisible:
		break;
	case ECloakingLevel::Vanished:
		if (GetController())
		{
			GetController()->SetIgnoreMoveInput(true);
		}
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		GetMesh()->SetVisibility(false);
		break;
	default:
		break;
	}
}

void AAP_FightUnit::HandleCloakFinished(ECloakingLevel Level)
{
	switch (Level)
	{
	case ECloakingLevel::Cloaked:
		break;
	case ECloakingLevel::Invisible:
		break;
	case ECloakingLevel::Vanished:
		if (GetController())
		{
			GetController()->SetIgnoreMoveInput(false);
		}
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		GetMesh()->SetVisibility(true);
		break;
	default:
		break;
	}
}

void AAP_FightUnit::GrantBountyExp()
{
	TArray<AActor*> Enemies;
	GetAllEnemyInRange(Enemies, EXP_AQUIRING_RANGE);
	UE_LOG_FAST(TEXT("AAP_Hero::GrantBountyExp got %d enemies"), Enemies.Num());
	if (!Enemies.Num())
	{
		return;
	}
	// exp given per unit gets smaller as enemy count grows, but not linearly
	float ExpPerUnit = Stats->GetBountyExp() / FMath::Sqrt(Enemies.Num());
	for (int i = Enemies.Num() - 1; i >= 0; i--)
	{
		auto Enemy = Cast<AAP_FightUnit>(Enemies[i]);
		if (!Enemy)
		{
			continue;
		}
		Enemy->Stats->GiveExp(ExpPerUnit);
	}
}

void AAP_FightUnit::LevelUpAbility(int AbilitySlot)
{
	float points = Stats->GetAbilityPoint();
	if(points < 1.0f)
	{
		return;
	}
	int level = AbilityUser->GetAbilityLevel(AbilitySlot);
	if (level >= ABILITY_MAX_LEVEL)
	{
		return;
	}
	AbilityUser->LevelUpAbility(AbilitySlot);
	Stats->SetAbilityPoint(points - 1.0f);
}

bool AAP_FightUnit::IsDead()
{
	if (!Stats)
	{
		return true;
	}
	return Stats->GetHealth() <= 0.0f;
}

void AAP_FightUnit::HandleDeath()
{
	AbilitySystem->CancelAllAbilities();
	// TODO: remove all timed buff
	GetCharacterMovement()->Deactivate();
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetVisibility(false);
	GetMesh()->Deactivate();
	float DeathTime = Stats->GetDeathTime();
	if (GetController())
	{
		GetController()->SetIgnoreMoveInput(true);
	}
	GrantBountyExp();
	OnDeath.Broadcast(DeathTime);
	GetWorldTimerManager().SetTimer(DeathTimerHandle, this, &AAP_FightUnit::Respawn, DeathTime, false, -1.0f);
}

void AAP_FightUnit::Respawn()
{
	GetWorldTimerManager().ClearTimer(DeathTimerHandle);
	GetCharacterMovement()->Activate();
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetMesh()->SetVisibility(true);
	GetMesh()->Activate();
	if (GetController())
	{
		GetController()->SetIgnoreMoveInput(false);
	}
	Stats->SetHealth(Stats->GetMaxHealth());
	Stats->SetMana(Stats->GetMaxMana());
	OnRespawn.Broadcast();
}

void AAP_FightUnit::HandleLevelUp()
{
}

bool AAP_FightUnit::LineTraceEnemy(FVector Start, FVector Direction, AActor*& OutActor)
{
	auto controller = Cast<AAP_RPGPlayerController>(GetController());
	check(controller);
	ECollisionChannel channel = controller->EnemyChannel;
	FHitResult hit_result;
	FVector start = Start;
	FVector end = Start + Direction * RAY_LENGTH;
	FCollisionQueryParams params = FCollisionQueryParams::DefaultQueryParam;
	bool hit = GetWorld()->LineTraceSingleByChannel(hit_result, start, end, channel, params);
	OutActor = hit_result.GetActor();
	UE_LOG(LogTemp, Warning, TEXT("UAP_AbilityBase::LineTraceUnit Start (%s) End (%s) channel %d, hit %d"), *Start.ToString(), *end.ToString(), channel, hit);
#ifdef ENABLE_DRAW_DEBUG
	FVector end_point = hit_result.Actor.IsValid() ? hit_result.ImpactPoint : hit_result.TraceEnd;
	DrawDebugSphere(GetWorld(), end_point, 16, 10, FColor::Green, false);
#endif // ENABLE_DRAW_DEBUG
	return hit;
}

bool AAP_FightUnit::LineTraceAlly(FVector Start, FVector Direction, AActor*& OutActor, bool IgnoreMe)
{
	auto controller = Cast<AAP_RPGPlayerController>(GetController());
	check(controller);
	ECollisionChannel channel = controller->AllyChannel;
	FHitResult hit_result;
	FVector start = Start;
	FVector end = Start + Direction * RAY_LENGTH;
	FCollisionQueryParams params = FCollisionQueryParams::DefaultQueryParam;
	if (IgnoreMe)
	{
		params.AddIgnoredActor(this);
	}
	bool hit = GetWorld()->LineTraceSingleByChannel(hit_result, start, end, channel, params);
	OutActor = hit_result.GetActor();
	UE_LOG(LogTemp, Warning, TEXT("UAP_AbilityBase::LineTraceUnit Start (%s) End (%s) Channel %d, hit %d"), *Start.ToString(), *end.ToString(), channel, hit);
#ifdef ENABLE_DRAW_DEBUG
	FVector end_point = hit_result.Actor.IsValid() ? hit_result.ImpactPoint : hit_result.TraceEnd;
	DrawDebugSphere(GetWorld(), end_point, 16, 10, FColor::Green, false);
#endif // ENABLE_DRAW_DEBUG
	return hit;
}

bool AAP_FightUnit::LineTraceUnitAuto(ETargetingPolicy Targeting, FVector Start, FVector Direction, AActor*& OutActor)
{
	switch (Targeting)
	{
	case ETargetingPolicy::UnitAll:
		return LineTraceUnit(Start, Direction, OutActor, false);
	case ETargetingPolicy::UnitExcludeSelf:
		return LineTraceUnit(Start, Direction, OutActor, true);
	case ETargetingPolicy::UnitAlly:
		return LineTraceAlly(Start, Direction, OutActor, false);
	case ETargetingPolicy::UnitAllyExcludeSelf:
		return LineTraceAlly(Start, Direction, OutActor, true);
	case ETargetingPolicy::UnitEnemy:
		return LineTraceEnemy(Start, Direction, OutActor);
	default:
		break;
	}
	return false;
}

bool AAP_FightUnit::LineTraceUnit(FVector Start, FVector Direction, AActor*& OutActor, bool IgnoreMe)
{
	auto controller = Cast<AAP_RPGPlayerController>(GetController());
	check(controller);
	ECollisionChannel channel = controller->AllUnitChannel;
	FHitResult hit_result;
	FVector start = Start;
	FVector end = Start + Direction * RAY_LENGTH;
	FCollisionQueryParams params = FCollisionQueryParams::DefaultQueryParam;
	if (IgnoreMe)
	{
		params.AddIgnoredActor(this);
	}
	bool hit = GetWorld()->LineTraceSingleByChannel(hit_result, start, end, channel, params);
	OutActor = hit_result.GetActor();
	UE_LOG(LogTemp, Warning, TEXT("UAP_AbilityBase::LineTraceUnit Start (%s) End (%s) Channel %d, hit %d"), *Start.ToString(), *end.ToString(), channel, hit);
#ifdef ENABLE_DRAW_DEBUG
	FVector end_point = hit_result.Actor.IsValid() ? hit_result.ImpactPoint : hit_result.TraceEnd;
	if (HasAuthority())
	{
		DrawDebugSphere(GetWorld(), end_point, 16, 10, FColor::Green, false);
	}
	else
	{
		DrawDebugSphere(GetWorld(), end_point, 16, 10, FColor::Blue, false);
	}
#endif // ENABLE_DRAW_DEBUG
	return hit;
}

bool AAP_FightUnit::LineTraceGround(FVector Start, FVector Direction, FVector& OutLocation)
{
	auto controller = Cast<AAP_RPGPlayerController>(GetController());
	check(controller);
	ECollisionChannel channel = controller->GroundChannel;
	FHitResult hit_result;
	FVector start = Start;
	FVector end = Start + Direction * RAY_LENGTH;
	bool hit = GetWorld()->LineTraceSingleByChannel(hit_result, start, end, channel);
	OutLocation = hit_result.Location;
#ifdef ENABLE_DRAW_DEBUG
	if (HasAuthority())
	{
		DrawDebugSphere(GetWorld(), OutLocation, 16, 10, FColor::Green, false);
	}
	else
	{
		DrawDebugSphere(GetWorld(), OutLocation, 16, 10, FColor::Blue, false);
	}
#endif // ENABLE_DRAW_DEBUG
	return hit;
}
void AAP_FightUnit::GetAllUnitInRange(TArray<AActor*>& Result, float Radius, bool IgnoreMe)
{
	auto controller = Cast<AAP_RPGPlayerController>(GetController());
	if (!controller)
	{
		return;
	}
	ECollisionChannel channel = controller->AllUnitChannel;
	FCollisionQueryParams Params = FCollisionQueryParams::DefaultQueryParam;
	if (IgnoreMe)
	{
		Params.AddIgnoredActor(this);
	}
	TArray<FOverlapResult> Overlaps;
	FCollisionShape Shape = FCollisionShape::MakeSphere(Radius);
	GetWorld()->OverlapMultiByChannel(Overlaps, GetActorLocation(), FQuat::Identity, channel, Shape, Params);
#if ENABLE_DRAW_DEBUG
	DrawDebugSphere(GetWorld(), GetActorLocation(), Radius, 10, FColor::Green, false, 1.5f);
#endif
	for (auto Overlap : Overlaps)
	{
		if (!Overlap.bBlockingHit)
		{
			continue;
		}
		auto Unit = Cast<IGenericTeamAgentInterface>(Overlap.Actor);
		if (!Unit)
		{
			continue;
		}
		Result.AddUnique(Overlap.Actor.Get());
	}
	UE_LOG_FAST(TEXT("AAP_Hero::GetAllUnitInRange, at %s,  got %d"), *GetActorLocation().ToString(), Result.Num());
}

void AAP_FightUnit::GetAllEnemyInRange(TArray<AActor*>& Result, float Radius)
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

void AAP_FightUnit::GetAllAllyInRange(TArray<AActor*>& Result, float Radius)
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

void AAP_FightUnit::UseNewFightStyle(EFightStyle NewStyle)
{
	if (FightStyle == NewStyle)
	{
		return;
	}
	FightStyle = NewStyle;
	OnFightStyleChanged.Broadcast();
}