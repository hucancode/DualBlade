// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Engine/Classes/Engine/Texture2D.h"
#include "GameplayAbilities/Public/GameplayEffect.h"
#include "AP_AbilityBase.h"
#include "AP_GameplayItem.generated.h"

UENUM(BlueprintType)
enum class EItemCategory :uint8
{
	WeaponAxe,
	WeaponBow,
	WeaponDagger,
	WeaponDualSword,
	WeaponKatana,
	WeaponMagicStaff,
	WeaponSwordAndShield,
	Armor,
	Consumable
};

UCLASS(BlueprintType, Blueprintable)
class LINHIBLADE_API UAP_GameplayItem : public UObject
{
	GENERATED_BODY()
public:
	UAP_GameplayItem() :
		Price(0),
		MaxStack(1),
		CurrentStack(0),
		Icon(nullptr),
		Category(EItemCategory::Consumable)
	{}
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FName Name;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		uint8 Price;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		uint8 MaxStack;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		uint8 CurrentStack;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UTexture2D* Icon;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<TSubclassOf<UGameplayEffect> > PassiveEffects;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<TSubclassOf<UAP_AbilityBase> > GrantedAbilities;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		EItemCategory Category;
};
