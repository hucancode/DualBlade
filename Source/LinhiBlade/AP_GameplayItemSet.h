// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "AP_GameplayItem.h"
#include "AP_GameplayItemSet.generated.h"

USTRUCT(BlueprintType)
struct FStockItem
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TSubclassOf<UAP_GameplayItem> Item;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int Stock;
};
UCLASS()
class LINHIBLADE_API UAP_GameplayItemSet : public UDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FStockItem> Items;
public:
	TSubclassOf<UAP_GameplayItem> ItemAt(int Index);
	int PriceAt(int Index);
	int StockAt(int Index);
	TSubclassOf<UAP_GameplayItem> Take(int Index);
	void Give(TSubclassOf<UAP_GameplayItem> Item);
	void Give(int Index);
	bool IsValidIndex(int Index);
};
