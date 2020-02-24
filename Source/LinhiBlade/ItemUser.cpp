// Fill out your copyright notice in the Description page of Project Settings.

#include "ItemUser.h"
#include "Net/UnrealNetwork.h"
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
	CurrentWeapon = EWeaponCategory::None;
	BuyRange = 1000.0f;
}


// Called every frame
void UItemUser::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UItemUser::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UItemUser, Gold);
	DOREPLIFETIME(UItemUser, StashItems);
	DOREPLIFETIME(UItemUser, EquipedItems);
}

void UItemUser::BuyItem(UItemSeller* Seller, int Slot)
{
	if (!GetOwner()->IsValidLowLevel())
	{
		return;
	}
	if (!GetOwner()->HasAuthority())
	{
		return;
	}
	auto my_location = GetOwner()->GetActorLocation();
	auto shop_location = Seller->GetOwner()->GetActorLocation();
	if (FVector::Dist(my_location, shop_location) > BuyRange)
	{
		return;
	}
	TSubclassOf<UAP_GameplayItem> item;
	if (StashItems.Num() >= STASH_SLOT_COUNT)
	{
		return;
	}
	if (!Seller->SellItem(Slot, this, item))
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
		if (!Overlap.Actor.IsValid())
		{
			continue;
		}
		d = FVector::Dist(GetOwner()->GetActorLocation(), Overlap.Actor->GetActorLocation());
		if (d > dmin)
		{
			continue;
		}
		auto shop_component = Overlap.Actor->GetComponentByClass(UItemSeller::StaticClass());
		if (!shop_component)
		{
			continue;
		}
		shop = Overlap.Actor.Get();
		dmin = d;
	}
	LastSeenShop = shop;
	return shop;
}

int UItemUser::GetGold()
{
	return Gold;
}

void UItemUser::GiveGold(int Amount)
{
	if (!GetOwner()->IsValidLowLevel())
	{
		return;
	}
	if (!GetOwner()->HasAuthority())
	{
		return;
	}
	Gold += Amount;
	Gold = FMath::Max(0, Gold);
	RecheckGold();
}

void UItemUser::UnequipRemoveItem(int Slot)
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
	RecheckEquipment();
}

void UItemUser::RemoveItem(int Slot)
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
	RecheckStash();
}

void UItemUser::UnequipItem(int Slot)
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
	EquipedItems.RemoveAt(Slot);
	RecheckStash();
	RecheckEquipment();
}

void UItemUser::GiveItem(TSubclassOf<UAP_GameplayItem> Item)
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
	StashItems.Add(Item);
	RecheckStash();
	UE_LOG_FAST(TEXT("give item success, now we have %d items"), StashItems.Num());
}

void UItemUser::EquipItem(int Slot)
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
	if (EquipedItems.Num() >= EQUIPMENT_SLOT_COUNT)
	{
		return;
	}
	StashItems.RemoveAt(Slot);
	EquipedItems.Add(item);
	RecheckStash();
	RecheckEquipment();
}

void UItemUser::GiveEquipItem(TSubclassOf<UAP_GameplayItem> Item)
{
	if (!GetOwner()->IsValidLowLevel())
	{
		return;
	}
	if (!GetOwner()->HasAuthority())
	{
		return;
	}
	EquipedItems.Add(Item);
	RecheckEquipment();
}

void UItemUser::UpdateWeapon()
{
	auto new_weapon = EWeaponCategory::None;
	UAP_GameplayItem* new_weapon_object = nullptr;
	for (auto item_class : EquipedItems)
	{
		auto item = item_class.GetDefaultObject();
		if (!item)
		{
			continue;
		}
		new_weapon = item->Category == EItemCategory::WeaponAxe ? EWeaponCategory::Axe :
			item->Category == EItemCategory::WeaponBow ? EWeaponCategory::Bow :
			item->Category == EItemCategory::WeaponDagger ? EWeaponCategory::Dagger :
			item->Category == EItemCategory::WeaponDualSword ? EWeaponCategory::DualSword :
			item->Category == EItemCategory::WeaponKatana ? EWeaponCategory::Katana :
			item->Category == EItemCategory::WeaponMagicStaff ? EWeaponCategory::MagicStaff :
			item->Category == EItemCategory::WeaponSwordAndShield ? EWeaponCategory::SwordAndShield : 
			EWeaponCategory::None;
		if (new_weapon != EWeaponCategory::None)
		{
			new_weapon_object = item;
			break;
		}
	}
	if (new_weapon != CurrentWeapon)
	{
		CurrentWeapon = new_weapon;
		OnWeaponChanged.Broadcast();
	}
	if (new_weapon_object != CurrentWeaponObject)
	{
		RemoveWeaponAbility(CurrentWeaponObject);
		CurrentWeaponObject = new_weapon_object;
		GiveWeaponAbility(CurrentWeaponObject);
	}
}

void UItemUser::OnRep_Gold()
{
	RecheckGold();
}

void UItemUser::RecheckGold()
{
	OnGoldChanged.Broadcast();
}

void UItemUser::OnRep_StashItems()
{
	RecheckStash();
}

void UItemUser::RecheckStash()
{
	OnStashChanged.Broadcast();
}

void UItemUser::OnRep_EquipedItems()
{
	RecheckEquipment();
}

void UItemUser::RecheckEquipment()
{
	OnEquipmentChanged.Broadcast();
	UpdateWeapon();
}

void UItemUser::GiveWeaponAbility(UAP_GameplayItem* Item)
{
	if (!GetOwner()->IsValidLowLevel())
	{
		return;
	}
	if (!Item)
	{
		for (auto Ability : BareHandAbilities)
		{
			OnAbilityAdded.Broadcast(Ability);
		}
	}
	else
	{
		for (auto Ability : Item->GrantedAbilities)
		{
			OnAbilityAdded.Broadcast(Ability);
		}
	}
}

void UItemUser::RemoveWeaponAbility(UAP_GameplayItem* Item)
{
	if (!GetOwner()->IsValidLowLevel())
	{
		return;
	}
	if (!Item)
	{
		for (auto Ability : BareHandAbilities)
		{
			OnAbilityRemoved.Broadcast(Ability);
		}
	}
	else
	{
		for (auto Ability : Item->GrantedAbilities)
		{
			OnAbilityRemoved.Broadcast(Ability);
		}
	}
}
