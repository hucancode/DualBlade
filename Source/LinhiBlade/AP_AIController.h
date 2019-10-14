// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "AP_AIController.generated.h"



UCLASS()
class LINHIBLADE_API AAP_AIController : public AAIController
{
	GENERATED_BODY()
public:
	virtual void SetGenericTeamId(const FGenericTeamId& Id) override;
	virtual FGenericTeamId GetGenericTeamId() const override;
};
