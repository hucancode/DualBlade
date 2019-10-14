// Fill out your copyright notice in the Description page of Project Settings.


#include "AP_AIController.h"
#include <LinhiBlade\AP_Hero.h>
#include <LinhiBlade\AP_GameMode.h>

void AAP_AIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	AAP_Hero* Hero = Cast<AAP_Hero>(InPawn);
	if (Hero)
	{
		if (Hero->HasMatchingGameplayTag(
			FGameplayTag::RequestGameplayTag("Combat.Team_1")))
		{
			SetGenericTeamId((uint8)EGameTeam::Team1);
		}
		else if (Hero->HasMatchingGameplayTag(
			FGameplayTag::RequestGameplayTag("Combat.Team_2")))
		{
			SetGenericTeamId((uint8)EGameTeam::Team2);
		}
		else
		{
			//
			SetGenericTeamId((uint8)EGameTeam::Neutral);
		}
	}
}

void AAP_AIController::SetGenericTeamId(const FGenericTeamId& Id)
{
	UE_LOG(LogTemp, Warning, TEXT("AAP_AIController::OnPossess set team %d"), (uint8)Id);
	TeamId = Id;
}

FGenericTeamId AAP_AIController::GetGenericTeamId() const
{
	//UE_LOG(LogTemp, Warning, TEXT("AAP_AIController::GetGenericTeamId TeamId %d"), (uint8)TeamId);
	return TeamId;
}

ETeamAttitude::Type AAP_AIController::GetTeamAttitudeTowards(const AActor& Other) const
{
	const IGenericTeamAgentInterface* OtherTeamAgent = Cast<const IGenericTeamAgentInterface>(Other.GetInstigatorController());
	ETeamAttitude::Type ret = OtherTeamAgent ? FGenericTeamId::GetAttitude(GetGenericTeamId(), OtherTeamAgent->GetGenericTeamId())
		: ETeamAttitude::Neutral;
	//UE_LOG(LogTemp, Warning, TEXT("AAP_AIController::GetTeamAttitudeTowards Other %s (%x) return %d"), *Other.GetName(), OtherTeamAgent,  (uint8)ret);
	return ret;
}
