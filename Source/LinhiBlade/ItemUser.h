// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LinhiBlade/AP_GameplayItem.h"
#include "ItemUser.generated.h"

#define EQUIPMENT_SLOT_COUNT 6
#define STASH_SLOT_COUNT 8

UENUM(BlueprintType)
enum class EWeaponCategory :uint8
{
	None = 0,
	Axe = 1,
	Bow = 2,
	Dagger = 3,
	DualSword = 4,
	Katana = 5,
	MagicStaff = 6,
	SwordAndShield = 7
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class LINHIBLADE_API UItemUser : public UActorComponent
{
	GENERATED_BODY()

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FItemAbilityEventDelegate, TSubclassOf<UAP_AbilityBase>, Ability);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FGenericEventDelegate);
public:	
	// Sets default values for this component's properties
	UItemUser();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRep_Gold)
		int Gold;
	UPROPERTY()
		AActor* LastSeenShop;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		float BuyRange;
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_StashItems)
		TArray<TSubclassOf<UAP_GameplayItem>> StashItems;
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_EquipedItems)
		TArray<TSubclassOf<UAP_GameplayItem>> EquipedItems;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		EWeaponCategory CurrentWeapon;
	UPROPERTY(BlueprintReadOnly)
		UAP_GameplayItem* CurrentWeaponObject;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<TSubclassOf<UAP_AbilityBase> > BareHandAbilities;

	UPROPERTY(BlueprintAssignable)
		FItemAbilityEventDelegate OnAbilityAdded;
	UPROPERTY(BlueprintAssignable)
		FItemAbilityEventDelegate OnAbilityRemoved;
	UPROPERTY(BlueprintAssignable)
		FGenericEventDelegate OnEquipmentChanged;
	UPROPERTY(BlueprintAssignable)
		FGenericEventDelegate OnStashChanged;
	UPROPERTY(BlueprintAssignable)
		FGenericEventDelegate OnGoldChanged;
	UPROPERTY(BlueprintAssignable)
		FGenericEventDelegate OnWeaponChanged;
	
	UFUNCTION(BlueprintCallable, Category = "Item")
		void BuyItem(class UItemSeller* Seller, int Slot);

	UFUNCTION(BlueprintPure)
		AActor* FindShop();

	UFUNCTION(BlueprintPure)
		int GetGold();

	UFUNCTION(BlueprintCallable, Category = "Item")
		void GiveGold(int Amount);

	UFUNCTION(BlueprintCallable, Category = "Item")
		void GiveItem(TSubclassOf<UAP_GameplayItem> Item);
	UFUNCTION(BlueprintCallable, Category = "Item")
		void GiveWeaponAbility(UAP_GameplayItem* Item);
	UFUNCTION(BlueprintCallable, Category = "Item")
		void RemoveWeaponAbility(UAP_GameplayItem* Item);
	UFUNCTION(BlueprintCallable, Category = "Item")
		void EquipItem(int Slot);

	UFUNCTION(BlueprintCallable, Category = "Item")
		void GiveEquipItem(TSubclassOf<UAP_GameplayItem> Item);

	UFUNCTION(BlueprintCallable, Category = "Item")
		void UnequipItem(int Slot);

	UFUNCTION(BlueprintCallable, Category = "Item")
		void RemoveItem(int Slot);

	UFUNCTION(BlueprintCallable, Category = "Item")
		void UnequipRemoveItem(int Slot);
	void UpdateWeapon();
protected:
	UFUNCTION()
		virtual void OnRep_Gold();
	void RecheckGold();
	UFUNCTION()
		virtual void OnRep_StashItems();
	void RecheckStash();
	UFUNCTION()
		virtual void OnRep_EquipedItems();
	void RecheckEquipment();
};
