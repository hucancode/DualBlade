// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AP_FightDefination.h"
#include "AP_PlayerState.generated.h"


UCLASS()
class LINHIBLADE_API AAP_PlayerState : public APlayerState
{
	GENERATED_BODY()

public:
    virtual void SeamlessTravelTo(class APlayerState* NewPlayerState) override;
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		EBodyType BodyType;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		ERace Race;
};
