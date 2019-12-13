// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemSeller.h"

// Sets default values for this component's properties
UItemSeller::UItemSeller()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UItemSeller::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UItemSeller::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

FShopItem UItemSeller::SellItem(int ShopSlot)
{
	if (!Items.IsValidIndex(ShopSlot))
	{
		return FShopItem();
	}
	if (!Items[ShopSlot].Stock)
	{
		return FShopItem();
	}
	Items[ShopSlot].Stock -= 1;
	return Items[ShopSlot];
}

void UItemSeller::RetriveItem(TSubclassOf<UAP_GameplayItem> Item)
{
	for (size_t i = 0; i < Items.Num(); i++)
	{
		if (Items[i].Item->GetClass() == Item)
		{
			Items[i].Stock++;
		}
	}
}

void UItemSeller::IncreaseStock(int ShopSlot)
{
	if (!Items.IsValidIndex(ShopSlot))
	{
		return;
	}
	Items[ShopSlot].Stock++;
}

