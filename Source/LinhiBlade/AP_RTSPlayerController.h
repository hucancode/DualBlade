// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "AP_Hero.h"
#include "AP_GameMode.h"

#include "AP_RTSPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class LINHIBLADE_API AAP_RTSPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	AAP_RTSPlayerController();
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = RTS)
	EGameTeam Team;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = RTS)
	FGameplayTag TeamTag;
	UFUNCTION(BlueprintCallable, Category = RTS)
		ETeamAttitude::Type GetAttituteTowardPlayer(AAP_RTSPlayerController* Other);
	UFUNCTION(BlueprintCallable, Category = RTS)
		void SetTeam(EGameTeam NewTeam);
};
