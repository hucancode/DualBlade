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
	
	ETeamAttitude::Type ret;
	if (From == Toward)
	{
		ret = ETeamAttitude::Friendly;
	}
	else if (From.GetId() == (uint8)EGameTeam::Neutral || 
		Toward.GetId() == (uint8)EGameTeam::Neutral)
	{
		ret = ETeamAttitude::Neutral;
	}
	else
	{
		ret = ETeamAttitude::Hostile;
	}
	UE_LOG(LogTemp, Warning, TEXT("AAP_GameMode::GetAttitude %d %d %d"), (uint8)From, (uint8)Toward, (uint8)ret);
	return ret;
}
