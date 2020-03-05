// Fill out your copyright notice in the Description page of Project Settings.


#include "AP_PlayerState.h"

void AAP_PlayerState::SeamlessTravelTo(APlayerState* NewPlayerState)
{
	Super::SeamlessTravelTo(NewPlayerState);
	auto state = Cast<AAP_PlayerState>(NewPlayerState);
	if(!state)
	{
		return;
	}
	this->BodyType = state->BodyType;
	this->Race = state->Race;
}
