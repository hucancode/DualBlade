// Fill out your copyright notice in the Description page of Project Settings.


#include "AP_CharacterSelector.h"

void AAP_CharacterSelector::TravelToActionPhase(FString MapName)
{
	if (HasAuthority())
	{
		GetWorld()->ServerTravel(MapName);
	}
	/*else
	{
		ClientTravel(MapName, TRAVEL_Relative, true);
	}*/
}
