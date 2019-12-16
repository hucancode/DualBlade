// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LinhiBlade/AP_GameplayItem.h"
#include "ItemUser.generated.h"

#define EQUIPMENT_SLOT_COUNT 6
#define STASH_SLOT_COUNT 8

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class LINHIBLADE_API UItemUser : public UActorComponent
{
	GENERATED_BODY()

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FItemAbilityEventDelegate, TSubclassOf<UAP_AbilityBase>, Ability);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FItemEventDelegate, UAP_GameplayItem*, Item);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGoldEventDelegate, int, Gold);
public:	
	// Sets default values for this component's properties
	UItemUser();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int Gold;
	UPROPERTY()
		AActor* LastSeenShop;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float BuyRange;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		TArray<UAP_GameplayItem*> StashItems;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		TArray<UAP_GameplayItem*> EquipedItems;

	UPROPERTY(BlueprintAssignable)
		FItemAbilityEventDelegate OnAbilityAdded;
	UPROPERTY(BlueprintAssignable)
		FItemAbilityEventDelegate OnAbilityRemoved;
	UPROPERTY(BlueprintAssignable)
		FItemEventDelegate OnItemEquipped;
	UPROPERTY(BlueprintAssignable)
		FItemEventDelegate OnItemUnequipped;
	UPROPERTY(BlueprintAssignable)
		FItemEventDelegate OnItemGiven;
	UPROPERTY(BlueprintAssignable)
		FItemEventDelegate OnItemRemoved;
	UPROPERTY(BlueprintAssignable)
		FGoldEventDelegate OnGoldChange;
	
	UFUNCTION(BlueprintCallable, NetMulticast, Reliable, Category = "Item")
		void BuyItem(class UItemSeller* Seller, int Slot);

	UFUNCTION()
		void BuyItemUncheck(UItemSeller* Seller, int Slot);

	UFUNCTION(BlueprintPure)
		AActor* FindShop();

	UFUNCTION(BlueprintPure)
		int GetGold();

	UFUNCTION(BlueprintCallable, NetMulticast, Reliable, Category = "Item")
		void GiveGold(int Amount);

	UFUNCTION(BlueprintCallable, NetMulticast, Reliable, Category = "Item")
		void GiveItem(TSubclassOf<UAP_GameplayItem> Item);

	UFUNCTION(BlueprintCallable, NetMulticast, Reliable, Category = "Item")
		void EquipItem(int Slot);

	UFUNCTION(BlueprintCallable, NetMulticast, Reliable, Category = "Item")
		void GiveEquipItem(TSubclassOf<UAP_GameplayItem> Item);

	UFUNCTION(BlueprintCallable, NetMulticast, Reliable, Category = "Item")
		void UnequipItem(int Slot);

	UFUNCTION(BlueprintCallable, NetMulticast, Reliable, Category = "Item")
		void RemoveItem(int Slot);

	UFUNCTION(BlueprintCallable, NetMulticast, Reliable, Category = "Item")
		void UnequipRemoveItem(int Slot);
};
