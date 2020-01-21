// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GenericTeamAgentInterface.h"
#include "GameFramework/GameModeBase.h"
#include "AP_GameMode.generated.h"

UENUM(BlueprintType)
enum class EGameTeam : uint8
{
	NoTeam = 0,
	Neutral = 1,
	Team1 = 2,
	Team2 = 3
};

UCLASS()
class LINHIBLADE_API AAP_GameMode : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	AAP_GameMode();
	virtual void StartPlay();
	UFUNCTION(Category = "Artificial Intelligence", BlueprintPure)
	static ETeamAttitude::Type GetAttitude(FGenericTeamId From, FGenericTeamId Toward);
public:
	UFUNCTION(BlueprintPure)
		ECollisionChannel GetTraceChannelAlly(EGameTeam Team);
	UFUNCTION(BlueprintPure)
		ECollisionChannel GetTraceChannelEnemy(EGameTeam Team);
	UFUNCTION(BlueprintPure)
		ECollisionChannel GetTraceChannelNeutral();
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TMap<EGameTeam, TEnumAsByte<ECollisionChannel>> TeamCollisionChannel;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TEnumAsByte<ECollisionChannel> GroundCollisionChannel;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TEnumAsByte<ECollisionChannel> UnitCollisionChannel;
};
