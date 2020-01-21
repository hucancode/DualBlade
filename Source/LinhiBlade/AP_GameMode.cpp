// Fill out your copyright notice in the Description page of Project Settings.


#include "AP_GameMode.h"
#include <AIController.h>

AAP_GameMode::AAP_GameMode()
{
	GroundCollisionChannel = ECollisionChannel::ECC_GameTraceChannel3;
	UnitCollisionChannel = ECollisionChannel::ECC_GameTraceChannel11;
	TeamCollisionChannel.Add(EGameTeam::Team1, ECollisionChannel::ECC_GameTraceChannel8);
	TeamCollisionChannel.Add(EGameTeam::Team2, ECollisionChannel::ECC_GameTraceChannel9);
	TeamCollisionChannel.Add(EGameTeam::Neutral, ECollisionChannel::ECC_GameTraceChannel10);
}
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

ECollisionChannel AAP_GameMode::GetTraceChannelAlly(EGameTeam Team)
{
	return Team == EGameTeam::Team1 ? TeamCollisionChannel[EGameTeam::Team1] :
		Team == EGameTeam::Team2 ? TeamCollisionChannel[EGameTeam::Team2] :
		ECC_MAX;
}

ECollisionChannel AAP_GameMode::GetTraceChannelEnemy(EGameTeam Team)
{
	return Team == EGameTeam::Team1 ? TeamCollisionChannel[EGameTeam::Team2] :
		Team == EGameTeam::Team2 ? TeamCollisionChannel[EGameTeam::Team1] :
		UnitCollisionChannel;
}
ECollisionChannel AAP_GameMode::GetTraceChannelNeutral()
{
	return TeamCollisionChannel[EGameTeam::Neutral];
}