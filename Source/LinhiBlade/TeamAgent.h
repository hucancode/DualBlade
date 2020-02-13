// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AP_GameMode.h"
#include <Runtime\AIModule\Classes\GenericTeamAgentInterface.h>
#include "TeamAgent.generated.h"

UENUM(BlueprintType)
enum class ECloakingLevel : uint8
{
	None		UMETA(DisplayName = "None"),
	Cloaked		UMETA(DisplayName = "Cloaked"),
	Invisible	UMETA(DisplayName = "Invisible"),
	Vanished	UMETA(DisplayName = "Vanished")
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class LINHIBLADE_API UTeamAgent : public UActorComponent
{
	GENERATED_BODY()

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTeamEventDelegate, EGameTeam, Team);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCloakEventDelegate, ECloakingLevel, Level);
public:	
	// Sets default values for this component's properties
	UTeamAgent();
protected:
	// Called when the game starts
	virtual void BeginPlay() override;
public:
	UPROPERTY(BlueprintAssignable)
		FTeamEventDelegate OnTeamChanged;
	UPROPERTY(BlueprintAssignable)
		FCloakEventDelegate OnCloakStarted;
	UPROPERTY(BlueprintAssignable)
		FCloakEventDelegate OnCloakFinished;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		ECloakingLevel CloakStatus;
	UPROPERTY()
		EGameTeam CurrentTeam;
public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	UFUNCTION(BlueprintCallable)
		void SetOverriddenController(AController* NewController);
	UFUNCTION(BlueprintCallable)
		void SetLogicalController(AController* NewController);
	UFUNCTION(BlueprintPure)
		AController* GetController();

	UFUNCTION(BlueprintCallable, NetMulticast, Reliable, Category = "Abilities")
		void EnterCloak(ECloakingLevel Level);
	UFUNCTION(BlueprintCallable, NetMulticast, Reliable, Category = "Abilities")
		void QuitCloak();
	// blueprint proxy function
	UFUNCTION(BlueprintCallable)
		void SetTeam(const EGameTeam Id);
	// blueprint proxy function
	UFUNCTION(BlueprintCallable)
		EGameTeam GetTeam();
	// blueprint proxy function
	UFUNCTION(BlueprintCallable)
		bool IsAlly();
	UFUNCTION(BlueprintCallable)
		bool IsHostile();
	UFUNCTION(BlueprintCallable, Category = "Abilities")
		bool IsVisible();
	UFUNCTION(BlueprintCallable)
		bool IsUnderControlOf(AController* OtherController);
	UFUNCTION(BlueprintCallable)
		bool IsUnderMyControl();
protected:
	AController* LogicalController;
	AController* OverriddenController;
};
