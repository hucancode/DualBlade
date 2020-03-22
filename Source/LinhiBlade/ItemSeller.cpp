// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemSeller.h"
#include "ItemUser.h"

// Sets default values for this component's properties
UItemSeller::UItemSeller()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	bWantsInitializeComponent = true;
	// ...
}


void UItemSeller::InitializeComponent()
{
	Super::InitializeComponent();
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

bool UItemSeller::SellItem(int ShopSlot, UItemUser* User, TSubclassOf<UAP_GameplayItem>& OutItem)
{
	if (!Items->IsValidIndex(ShopSlot))
	{
		return false;
	}
	if (!Items->StockAt(ShopSlot))
	{
		return false;
	}
	auto price = Items->PriceAt(ShopSlot);
	if (User->GetGold() < Items->PriceAt(ShopSlot))
	{
		return false;
	}
	User->GiveGold(-price);
	OutItem = Items->Take(ShopSlot);
	return true;
}

void UItemSeller::ReturnItem(TSubclassOf<UAP_GameplayItem> Item, UItemUser* User)
{
	int price = Item.GetDefaultObject()->Price * RESELL_FALL_OFF_RATE;
	User->GiveGold(price);
	Items->Give(Item);
}
