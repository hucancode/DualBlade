// Fill out your copyright notice in the Description page of Project Settings.


#include "AP_GameMode.h"
#include <AIController.h>

void AAP_GameMode::StartPlay()
{
	Super::StartPlay();
	UE_LOG(LogTemp, Warning, TEXT("AAP_GameMode::StartPlay() SetAttitudeSolver"));
	FGenericTeamId::SetAttitudeSolver(&AAP_GameMode::GetAttitude);
}

ETeamAttitude::Type AAP_GameMode::GetAttitude(FGenericTeamId From, FGenericTeamId Toward)
{
	if (From == Toward)
	{
		return ETeamAttitude::Friendly;
	}
	if (From.GetId() == (uint8)EGameTeam::Neutral || 
		Toward.GetId() == (uint8)EGameTeam::Neutral)
	{
		return ETeamAttitude::Neutral;
	}
	return ETeamAttitude::Hostile;
}
