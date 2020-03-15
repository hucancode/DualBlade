// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "AP_CharacterSelector.generated.h"

/**
 * 
 */
UCLASS()
class LINHIBLADE_API AAP_CharacterSelector : public APlayerController
{
	GENERATED_BODY()

		UFUNCTION(BlueprintCallable, Category = RPG)
		void TravelToActionPhase(FString MapName);
};
