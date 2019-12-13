// Fill out your copyright notice in the Description page of Project Settings.


#include "AP_AIController.h"
#include <LinhiBlade\AP_GameMode.h>

void AAP_AIController::SetGenericTeamId(const FGenericTeamId& Id)
{
	IGenericTeamAgentInterface* Agent = Cast<IGenericTeamAgentInterface>(GetPawn());
	if (Agent)
	{
		Agent->SetGenericTeamId(Id);
	}
	else
	{
		AAIController::SetGenericTeamId(Id);
	}
}

FGenericTeamId AAP_AIController::GetGenericTeamId() const
{
	const IGenericTeamAgentInterface* Agent = Cast<const IGenericTeamAgentInterface>(GetPawn());
	if (Agent)
	{
		return Agent->GetGenericTeamId();
	}
	return AAIController::GetGenericTeamId();
}