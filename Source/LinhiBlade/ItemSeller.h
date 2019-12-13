// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LinhiBlade/AP_GameplayItem.h"
#include "ItemSeller.generated.h"

USTRUCT(BlueprintType)
struct FShopItem
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		UAP_GameplayItem* Item;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		int Price;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		int Stock;
	FShopItem()
	{
		Stock = -1;
		Price = 0;
	}
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class LINHIBLADE_API UItemSeller : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UItemSeller();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		TArray<FShopItem> Items;
	UFUNCTION(BlueprintCallable)
		FShopItem SellItem(int ShopSlot);
	UFUNCTION()
		void RetriveItem(TSubclassOf<UAP_GameplayItem> Item);
	UFUNCTION()
		void IncreaseStock(int ShopSlot);
};
