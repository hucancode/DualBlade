// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectUIData.h"
#include "Engine/Classes/Engine/Texture2D.h"
#include "AP_GameplayEffectUIData_TextIcon.generated.h"
/**
 * 
 */
UCLASS()
class LINHIBLADE_API UAP_GameplayEffectUIData_TextIcon : public UGameplayEffectUIData
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Data, meta = (MultiLine = "true"))
	FText Description;
	UTexture2D* Icon;
};
