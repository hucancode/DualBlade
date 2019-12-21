// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemUser.h"
#include "ItemSeller.h"

#ifndef UE_LOG_FAST
#define UE_LOG_FAST(Format, ...) UE_LOG(LogTemp, Display, Format, ##__VA_ARGS__)
#endif

// Sets default values for this component's properties
UItemUser::UItemUser()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UItemUser::BeginPlay()
{
	Super::BeginPlay();
	Gold = 0;
	LastSeenShop = nullptr;
	// ...
	
}


// Called every frame
void UItemUser::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UItemUser::BuyItem_Implementation(UItemSeller* Seller, int Slot)
{
	auto my_location = GetOwner()->GetActorLocation();
	auto shop_location = Seller->GetOwner()->GetActorLocation();
	if (FVector::Dist(my_location, shop_location) > BuyRange)
	{
		return;
	}
	BuyItemUncheck(Seller, Slot);
}

void UItemUser::BuyItemUncheck(UItemSeller* Seller, int Slot)
{
	TSubclassOf<UAP_GameplayItem> item;
	if (StashItems.Num() >= STASH_SLOT_COUNT)
	{
		return;
	}
	if(!Seller->SellItem(Slot, this, item))
	{
		return;
	}
	GiveItem(item);
}

AActor* UItemUser::FindShop()
{
	TArray<FOverlapResult> Overlaps;
	auto Location = GetOwner()->GetActorLocation();
	auto Rotation = FQuat::Identity;
	auto Channel = ECollisionChannel::ECC_Pawn;
	auto Shape = FCollisionShape::MakeSphere(BuyRange);
	auto ObjectToScan = FCollisionObjectQueryParams(Channel);
	if (LastSeenShop && LastSeenShop->IsValidLowLevel())
	{
		if (FVector::Dist(Location, LastSeenShop->GetActorLocation()) <= BuyRange)
		{
			return LastSeenShop;
		}
	}
	//GetWorld()->OverlapMultiByChannel(Overlaps, Location, Rotation, Channel, Shape);
	GetWorld()->OverlapMultiByObjectType(Overlaps, Location, Rotation, ObjectToScan, Shape);
	AActor* shop = nullptr;
	float d, dmin = 9999999.0f;
	for (auto Overlap : Overlaps)
	{
		d = FVector::Dist(GetOwner()->GetActorLocation(), Overlap.Actor->GetActorLocation());
		if (d > dmin)
		{
			continue;
		}
		if (!Overlap.Actor->GetComponentByClass(UItemSeller::StaticClass()))
		{
			continue;
		}
		dmin = d;
		shop = Overlap.Actor.Get();
	}
	LastSeenShop = shop;
	return shop;
}

int UItemUser::GetGold()
{
	return Gold;
}

void UItemUser::GiveGold_Implementation(int Amount)
{
	Gold += Amount;
	Gold = FMath::Max(0, Gold);
	OnGoldChange.Broadcast(Gold);
}

void UItemUser::UnequipRemoveItem_Implementation(int Slot)
{
	if (!GetOwner()->IsValidLowLevel())
	{
		return;
	}
	if (!GetOwner()->HasAuthority())
	{
		return;
	}
	if (!EquipedItems.IsValidIndex(Slot))
	{
		return;
	}
	auto item = EquipedItems[Slot];
	EquipedItems.RemoveAt(Slot);
	OnItemUnequipped.Broadcast(item);
}

void UItemUser::RemoveItem_Implementation(int Slot)
{
	if (!GetOwner()->IsValidLowLevel())
	{
		return;
	}
	if (!GetOwner()->HasAuthority())
	{
		return;
	}
	if (!StashItems.IsValidIndex(Slot))
	{
		return;
	}
	auto item = StashItems[Slot];
	StashItems.RemoveAt(Slot);
	OnItemRemoved.Broadcast(item);
}

void UItemUser::UnequipItem_Implementation(int Slot)
{
	if (!GetOwner()->IsValidLowLevel())
	{
		return;
	}
	if (!GetOwner()->HasAuthority())
	{
		return;
	}
	if (!EquipedItems.IsValidIndex(Slot))
	{
		return;
	}
	if (StashItems.Num() >= STASH_SLOT_COUNT)
	{
		return;
	}
	auto item = EquipedItems[Slot];
	StashItems.Add(item);
	for (auto Ability : item->GrantedAbilities)
	{
		OnAbilityRemoved.Broadcast(Ability);
	}
	EquipedItems.RemoveAt(Slot);
	OnItemUnequipped.Broadcast(item);
}

void UItemUser::GiveItem_Implementation(TSubclassOf<UAP_GameplayItem> Item)
{
	if (!GetOwner()->IsValidLowLevel())
	{
		return;
	}
	if (!GetOwner()->HasAuthority())
	{
		return;
	}
	if (StashItems.Num() >= STASH_SLOT_COUNT)
	{
		return;
	}
	UAP_GameplayItem* item = Item.GetDefaultObject();
	UAP_GameplayItem* ret = NewObject<UAP_GameplayItem>(GetOwner(), item->Name, RF_NoFlags, item);
	ret->CurrentStack = 1;
	StashItems.Add(ret);
	UE_LOG_FAST(TEXT("give item success, now we have %d items"), StashItems.Num());
	OnItemGiven.Broadcast(ret);
}

void UItemUser::EquipItem_Implementation(int Slot)
{
	if (!GetOwner()->IsValidLowLevel())
	{
		return;
	}
	if (!GetOwner()->HasAuthority())
	{
		return;
	}
	if (!StashItems.IsValidIndex(Slot))
	{
		return;
	}
	UAP_GameplayItem* item = StashItems[Slot];
	if (EquipedItems.Num() >= EQUIPMENT_SLOT_COUNT)
	{
		return;
	}
	StashItems.RemoveAt(Slot);
	for (auto Ability : item->GrantedAbilities)
	{
		UE_LOG_FAST(TEXT("OnAbilityAdded.Broadcast(Ability) %d"), OnAbilityAdded.IsBound());
		OnAbilityAdded.Broadcast(Ability);
		
	}
	EquipedItems.Add(item);
	OnItemEquipped.Broadcast(item);
	UE_LOG_FAST(TEXT("UItemUser::EquipItem_Implementation"));
}

void UItemUser::GiveEquipItem_Implementation(TSubclassOf<UAP_GameplayItem> Item)
{
	if (!GetOwner()->IsValidLowLevel())
	{
		return;
	}
	if (!GetOwner()->HasAuthority())
	{
		return;
	}
	UAP_GameplayItem* item = Item.GetDefaultObject();
	UAP_GameplayItem* ret = NewObject<UAP_GameplayItem>(GetOwner(), item->Name, RF_NoFlags, item);
	EquipedItems.Add(ret);
	for (auto Ability : item->GrantedAbilities)
	{
		OnAbilityAdded.Broadcast(Ability);
	}
	OnItemEquipped.Broadcast(ret);
}

