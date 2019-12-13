// Fill out your copyright notice in the Description page of Project Settings.


#include "TeamAgent.h"
#include "AP_RTSPlayerController.h"
#include "DrawDebugHelpers.h"

#ifndef UE_LOG_FAST
#define UE_LOG_FAST(Format, ...) UE_LOG(LogTemp, Display, Format, ##__VA_ARGS__)
#endif

// Sets default values for this component's properties
UTeamAgent::UTeamAgent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UTeamAgent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UTeamAgent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UTeamAgent::SetLogicalController(AController* NewController)
{
	auto RTSController = Cast<AAP_RTSPlayerController>(NewController);
	if (!RTSController->IsValidLowLevel())
	{
		return;
	}
	LogicalController = NewController;
	SetTeam(RTSController->Team);
}

void UTeamAgent::EnterCloak_Implementation(ECloakingLevel Level)
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
	OnCloakStarted.Broadcast(Level);
}

void UTeamAgent::QuitCloak_Implementation()
{
	auto LastStatus = CloakStatus;
	CloakStatus = ECloakingLevel::None;
	OnCloakFinished.Broadcast(LastStatus);
}

void UTeamAgent::SetTeam(const EGameTeam Id)
{
	if (CurrentTeam == Id)
	{
		return;
	}
	CurrentTeam = Id;
	OnTeamChanged.Broadcast(Id);
}

EGameTeam UTeamAgent::GetTeam()
{
	return CurrentTeam;
}

bool UTeamAgent::IsAlly()
{
	auto Me = Cast<AAP_RTSPlayerController>(GetWorld()->GetFirstPlayerController());
	auto ThisUnit = Cast<AAP_RTSPlayerController>(LogicalController);
	if (Me && ThisUnit)
	{
		return Me->GetAttituteTowardPlayer(ThisUnit) == ETeamAttitude::Type::Friendly;
	}
	return false;
}

bool UTeamAgent::IsHostile()
{
	auto Me = Cast<AAP_RTSPlayerController>(GetWorld()->GetFirstPlayerController());
	auto ThisUnit = Cast<AAP_RTSPlayerController>(LogicalController);
	if (Me && ThisUnit)
	{
		return Me->GetAttituteTowardPlayer(ThisUnit) == ETeamAttitude::Type::Hostile;
	}
	return false;
}

bool UTeamAgent::IsVisible()
{
	if (IsAlly())
	{
		return CloakStatus != ECloakingLevel::Vanished;
	}
	return CloakStatus == ECloakingLevel::None;
}

bool UTeamAgent::IsUnderControlOf(AController* OtherController)
{
	return OtherController == LogicalController;
}

bool UTeamAgent::IsUnderMyControl()
{
	auto Me = GetWorld()->GetFirstPlayerController();
	return IsUnderControlOf(Me);
}

void UTeamAgent::GetAllUnitInRange(TArray<AActor*>& Result, float Radius)
{
	const static ECollisionChannel Channel = ECollisionChannel::ECC_GameTraceChannel11;
	FCollisionQueryParams Params(SCENE_QUERY_STAT(GetAllUnitInRange), false);
	TArray<FOverlapResult> Overlaps;
	FCollisionShape Shape = FCollisionShape::MakeSphere(Radius);
	GetWorld()->OverlapMultiByChannel(Overlaps, GetOwner()->GetActorLocation(), FQuat::Identity, Channel, Shape, Params);
#if ENABLE_DRAW_DEBUG
	DrawDebugSphere(GetWorld(), GetOwner()->GetActorLocation(), Radius, 10, FColor::Green, false, 1.5f);
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
	UE_LOG_FAST(TEXT("AAP_Hero::GetAllUnitInRange, at %s,  got %d"), *GetOwner()->GetActorLocation().ToString(), Result.Num());
}

void UTeamAgent::GetAllEnemyInRange(TArray<AActor*>& Result, float Radius)
{
	auto owner = Cast<IGenericTeamAgentInterface>(GetOwner());
	if (!owner)
	{
		return;
	}
	GetAllUnitInRange(Result, Radius);
	for (int i = Result.Num() - 1; i >= 0; i--)
	{
		auto Unit = Result[i];
		if (owner->GetTeamAttitudeTowards(*Unit) == ETeamAttitude::Hostile)
		{
			continue;
		}
		Result.RemoveAt(i);
	}
}

void UTeamAgent::GetAllAllyInRange(TArray<AActor*>& Result, float Radius)
{
	auto owner = Cast<IGenericTeamAgentInterface>(GetOwner());
	if (!owner)
	{
		return;
	}
	GetAllUnitInRange(Result, Radius);
	for (int i = Result.Num() - 1; i >= 0; i--)
	{
		auto Unit = Result[i];
		if (owner->GetTeamAttitudeTowards(*Unit) == ETeamAttitude::Friendly)
		{
			continue;
		}
		Result.RemoveAt(i);
	}
}
