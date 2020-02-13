// Fill out your copyright notice in the Description page of Project Settings.


#include "TeamAgent.h"
#include "AP_RPGPlayerController.h"
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

void UTeamAgent::SetOverriddenController(AController* NewController)
{
	if (NewController)
	{
		auto RTSController = Cast<AAP_RPGPlayerController>(NewController);
		if (!RTSController->IsValidLowLevel())
		{
			return;
		}
		OverriddenController = NewController;
		SetTeam(RTSController->Team);
	}
	else
	{
		SetLogicalController(LogicalController);
	}
}

void UTeamAgent::SetLogicalController(AController* NewController)
{
	auto RTSController = Cast<AAP_RPGPlayerController>(NewController);
	if (!RTSController->IsValidLowLevel())
	{
		return;
	}
	LogicalController = NewController;
	SetTeam(RTSController->Team);
}

AController* UTeamAgent::GetController()
{
	if (OverriddenController)
	{
		return OverriddenController;
	}
	return LogicalController;
}

void UTeamAgent::EnterCloak_Implementation(ECloakingLevel Level)
{
	if (CloakStatus != Level && CloakStatus != ECloakingLevel::None)
	{
		QuitCloak();
	}
	if (Level == ECloakingLevel::None)
	{
		return;
	}
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
	auto Me = Cast<AAP_RPGPlayerController>(GetWorld()->GetFirstPlayerController());
	auto ThisUnit = Cast<AAP_RPGPlayerController>(GetController());
	if (Me && ThisUnit)
	{
		return Me->GetAttituteTowardPlayer(ThisUnit) == ETeamAttitude::Type::Friendly;
	}
	return false;
}

bool UTeamAgent::IsHostile()
{
	auto Me = Cast<AAP_RPGPlayerController>(GetWorld()->GetFirstPlayerController());
	auto ThisUnit = Cast<AAP_RPGPlayerController>(GetController());
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
	return OtherController == GetController();
}

bool UTeamAgent::IsUnderMyControl()
{
	auto Me = GetWorld()->GetFirstPlayerController();
	return IsUnderControlOf(Me);
}