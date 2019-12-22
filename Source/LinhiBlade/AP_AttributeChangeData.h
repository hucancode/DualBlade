// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "AP_AttributeSet.h"
#include "AP_AttributeChangeData.generated.h"

/**
 * 
 */
UCLASS()
class LINHIBLADE_API UAP_AttributeChangeData : public UObject
{
	GENERATED_BODY()
public:
	UPROPERTY()
	FGameplayAttribute Attribute;
	UPROPERTY()
	float NewValue;
	UPROPERTY()
	float OldValue;
	UPROPERTY()
	UAP_AttributeSet* AllAttribute;
};
