// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Templates/SubclassOf.h"
#include "Engine/DataAsset.h"
#include "Abilities/GameplayAbility.h"
#include "AP_GameplayAbilitySet.generated.h"

class UAbilitySystemComponent;

/**
 *	This is an example input binding enum for GameplayAbilities. Your project may want to create its own.
 *	The MetaData default bind keys (LMB, RMB, Q, E, etc) are a convenience for designers setting up abilities sets
 *	or whatever other data you have that gives an ability with a default key binding. Actual key binding is up to DefaultInput.ini
 *	
 *	E.g., "Ability1" is the command string that is bound to AbilitySystemComponent::ActivateAbility(1). The Meta data only *suggests*
 *	that you are binding "Ability1" to LMB by default in your projects DefaultInput.ini.
 */
UENUM(BlueprintType)
enum ESpellSlot
{
	Ability1 = 0			UMETA(DisplayName = "Ability1 (A)"),
	Ability2				UMETA(DisplayName = "Ability2 (Q)"),
	Ability3				UMETA(DisplayName = "Ability3 (W)"),
	Ability4				UMETA(DisplayName = "Ability4 (E)"),
	Ability5				UMETA(DisplayName = "Ability5 (R1)"),
	Ability6				UMETA(DisplayName = "Ability6 (R2)"),
	Ability7				UMETA(DisplayName = "Ability7 (R3)"),
	Ability8				UMETA(DisplayName = "Ability8 (R4)"),
	Ability9				UMETA(DisplayName = "Ability9 (R5)"),
	Ability10				UMETA(DisplayName = "Ability10 (D)"),
	Ability11				UMETA(DisplayName = "Ability11 (F)"),
	Ability_Invalid			UMETA(DisplayName = "Ability_Invalid"),
};

/**
 *	Example struct that pairs a enum input command to a GameplayAbilityClass.6
 */
USTRUCT()
struct FGameplayAbilitySlotInfo
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, Category = BindInfo)
	TEnumAsByte<ESpellSlot>	Slot;

	UPROPERTY(EditAnywhere, Category = BindInfo)
	TSubclassOf<UGameplayAbility>	GameplayAbilityClass;
};

/**
 *	This is an example DataAsset that could be used for defining a set of abilities to give to an AbilitySystemComponent and bind to an input command.
 *	Your project is free to implement this however it wants!
 *	
 *	
 */
UCLASS()
class LINHIBLADE_API UAP_GameplayAbilitySet : public UDataAsset
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	UAP_GameplayAbilitySet();
public:
	UPROPERTY(EditAnywhere, Category = AbilitySet)
	TMap< TEnumAsByte<ESpellSlot>, TSubclassOf<UGameplayAbility>>	Abilities;
	ESpellSlot Find(const UGameplayAbility* Ability);
};
