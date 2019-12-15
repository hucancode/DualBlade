// Fill out your copyright notice in the Description page of Project Settings.


#include "AP_GameplayItemSet.h"

TSubclassOf<UAP_GameplayItem> UAP_GameplayItemSet::ItemAt(int Index)
{
	return Items[Index].Item;
}

int UAP_GameplayItemSet::PriceAt(int Index)
{
	return ItemAt(Index).GetDefaultObject()->Price;
}

int UAP_GameplayItemSet::StockAt(int Index)
{
	return Items[Index].Stock;
}
TSubclassOf<UAP_GameplayItem> UAP_GameplayItemSet::Take(int Index)
{
	Items[Index].Stock--;
	return Items[Index].Item;
}
void UAP_GameplayItemSet::Give(int Index)
{
	Items[Index].Stock++;
}
void UAP_GameplayItemSet::Give(TSubclassOf<UAP_GameplayItem> Item)
{
	for (size_t i = 0; i < Items.Num(); i++)
	{
		if (Items[i].Item == Item)
		{
			Items[i].Stock++;
			break;
		}
	}
}

bool UAP_GameplayItemSet::IsValidIndex(int Index)
{
	return Items.IsValidIndex(Index);
}
