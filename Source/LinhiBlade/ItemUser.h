// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LinhiBlade/AP_GameplayItem.h"
#include "ItemSeller.h"
#include "ItemUser.generated.h"

#define EQUIPMENT_SLOT_COUNT 6
#define STASH_SLOT_COUNT 8

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class LINHIBLADE_API UItemUser : public UActorComponent
{
	GENERATED_BODY()

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FItemAbilityEventDelegate, TSubclassOf<UAP_AbilityBase>, Ability);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FItemEventDelegate, UAP_GameplayItem*, Item);
public:	
	// Sets default values for this component's properties
	UItemUser();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY()
		int Gold;

	UPROPERTY()
		TArray<UAP_GameplayItem*> StashItems;
	UPROPERTY()
		TArray<UAP_GameplayItem*> EquipedItems;

	UPROPERTY(BlueprintAssignable)
		FItemAbilityEventDelegate OnAbilityAdded;
	UPROPERTY(BlueprintAssignable)
		FItemAbilityEventDelegate OnAbilityRemoved;
	UPROPERTY(BlueprintAssignable)
		FItemEventDelegate OnItemEquipped;
	UPROPERTY(BlueprintAssignable)
		FItemEventDelegate OnItemRemoved;
	
	UFUNCTION(BlueprintCallable)
		void BuyItem(UItemSeller* Seller, int Slot);

	UFUNCTION(BlueprintCallable, NetMulticast, Reliable, Category = "Abilities")
		void GiveGold(int Amount);

	UFUNCTION(BlueprintCallable, NetMulticast, Reliable, Category = "Abilities")
		void GiveItem(TSubclassOf<UAP_GameplayItem> Item);

	UFUNCTION(BlueprintCallable, NetMulticast, Reliable, Category = "Abilities")
		void EquipItem(int Slot);

	UFUNCTION(BlueprintCallable, NetMulticast, Reliable, Category = "Abilities")
		void GiveEquipItem(TSubclassOf<UAP_GameplayItem> Item);

	UFUNCTION(BlueprintCallable, NetMulticast, Reliable, Category = "Abilities")
		void UnequipItem(int Slot);

	UFUNCTION(BlueprintCallable, NetMulticast, Reliable, Category = "Abilities")
		void RemoveItem(int Slot);

	UFUNCTION(BlueprintCallable, NetMulticast, Reliable, Category = "Abilities")
		void UnequipRemoveItem(int Slot);

};
