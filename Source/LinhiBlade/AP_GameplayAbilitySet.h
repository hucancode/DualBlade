// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Templates/SubclassOf.h"
#include "Engine/DataAsset.h"
#include "Abilities/GameplayAbility.h"
#include "AP_GameplayItem.h"
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
	UAP_GameplayAbilitySet();
public:
	UPROPERTY(EditAnywhere, Category = AbilitySet)
		TArray<TSubclassOf<UGameplayAbility> > Abilities;
};
