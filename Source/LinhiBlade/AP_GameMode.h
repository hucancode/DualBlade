// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GenericTeamAgentInterface.h"
#include "GameFramework/GameModeBase.h"
#include "AP_GameMode.generated.h"

UENUM(BlueprintType)
enum class EGameTeam : uint8
{
	Neutral = 0,
	Team1 = 1,
	Team2 = 2,
	NoTeam = 3
};

UCLASS()
class LINHIBLADE_API AAP_GameMode : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	virtual void StartPlay();
	UFUNCTION(Category = "Artificial Intelligence", BlueprintPure)
		static ETeamAttitude::Type GetAttitude(FGenericTeamId From, FGenericTeamId Toward);

};
