// Fill out your copyright notice in the Description page of Project Settings.


#include "AP_RTSPlayerController.h"

AAP_RTSPlayerController::AAP_RTSPlayerController()
{
	Team = EGameTeam::Neutral;
	TeamTag = FGameplayTag::RequestGameplayTag("Combat.Team_Neutral");
}
ETeamAttitude::Type AAP_RTSPlayerController::GetAttituteTowardPlayer(AAP_RTSPlayerController* Other)
{
	return FGenericTeamId::GetAttitude((uint8)Other->Team, (uint8)this->Team);
}

void AAP_RTSPlayerController::SetTeam(EGameTeam NewTeam)
{
	Team = NewTeam;
	TeamTag = Team == EGameTeam::Team1 ? FGameplayTag::RequestGameplayTag("Combat.Team_1") :
		Team == EGameTeam::Team2 ? FGameplayTag::RequestGameplayTag("Combat.Team_2") :
		FGameplayTag::RequestGameplayTag("Combat.Team_Neutral");
}
