// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LinhiBlade/AP_GameplayItem.h"
#include "AP_GameplayItemSet.h"
#include "ItemSeller.generated.h"

#define RESELL_FALL_OFF_RATE 0.5f
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
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UAP_GameplayItemSet* Items;
	UFUNCTION(BlueprintCallable)
		bool SellItem(int ShopSlot, class UItemUser* User, TSubclassOf<UAP_GameplayItem>& OutItem);
	UFUNCTION()
		void ReturnItem(TSubclassOf<UAP_GameplayItem> Item, class UItemUser* User);
};
