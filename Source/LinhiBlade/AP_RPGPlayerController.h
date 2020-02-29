// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "AP_GameMode.h"
#include "GameplayTagContainer.h"
#include "AP_RPGPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class LINHIBLADE_API AAP_RPGPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	AAP_RPGPlayerController();
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = RTS)
		EGameTeam Team;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = RTS)
		FGameplayTag TeamTag;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = RTS)
		TEnumAsByte<ECollisionChannel> GroundChannel;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = RTS)
		TEnumAsByte<ECollisionChannel> AllyChannel;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = RTS)
		TEnumAsByte<ECollisionChannel> EnemyChannel;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = RTS)
		TEnumAsByte<ECollisionChannel> NeutralChannel;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = RTS)
		TEnumAsByte<ECollisionChannel> AllUnitChannel;
	UFUNCTION(BlueprintCallable, Category = RPG)
		ETeamAttitude::Type GetAttituteTowardPlayer(AAP_RPGPlayerController* Other);
	UFUNCTION(BlueprintCallable, Category = RPG)
		void SetTeam(EGameTeam NewTeam);
	UFUNCTION(BlueprintCallable, Category = RPG)
		bool LineTraceGround(FVector Start, FVector Direction, FVector& OutLocation);
	UFUNCTION(BlueprintCallable, Category = RPG)
		void PlayerFaceTo(FVector Target);
	UFUNCTION(BlueprintCallable, Category = RPG)
		void PlayerFaceToMouse();
};
