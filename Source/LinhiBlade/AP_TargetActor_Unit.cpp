// Fill out your copyright notice in the Description page of Project Settings.

#include "AP_TargetActor_Unit.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "GameFramework/PlayerController.h"
#include "Abilities/GameplayAbility.h"
#include "DrawDebugHelpers.h"
#include "AbilitySystemInterface.h"

void AAP_TargetActor_Unit::BeginPlay()
{
	Super::BeginPlay();
	// see DefaultEngine.ini to get updated channel ID in case config changed
	if (bDebug)
	{
		PrimaryActorTick.bCanEverTick = true;
		PrimaryActorTick.SetTickFunctionEnable(true);
		PrimaryActorTick.TickGroup = TG_PostUpdateWork;
		// workaround for an engine bug, turn the tick function off and on again
		RegisterAllActorTickFunctions(false, false);
		RegisterAllActorTickFunctions(true, false);
		bDestroyOnConfirmation = true;
	}
}

void AAP_TargetActor_Unit::StartTargeting(UGameplayAbility* Ability)
{
	Super::StartTargeting(Ability);
	UE_LOG(LogTemp, Warning, TEXT("AAP_TargetActor_Unit::StartTargeting() %d"));
	GetWorld()->GetFirstPlayerController()->CurrentMouseCursor = EMouseCursor::Crosshairs;
	LastUnitHighlighted = nullptr;
}

void AAP_TargetActor_Unit::ConfirmTargetingAndContinue()
{
	// check if this is a local controller or this is server. currently crash so disabled
	//check(ShouldProduceTargetData());
	UE_LOG(LogTemp, Warning, TEXT("AAP_TargetActor_Unit::ConfirmTargetingAndContinue()"));
	if (IsConfirmTargetingAllowed())
	{	
		TArray <TWeakObjectPtr<AActor>> HitActors;
		FGameplayAbilityTargetDataHandle Handle;
		FHitResult HitResult = PerformTrace();
		TWeakObjectPtr<AActor> HitActor = HitResult.Actor;

		auto unit = Cast<IAbilitySystemInterface>(HitActor);
		if (unit)
		{
			UE_LOG(LogTemp, Warning, TEXT("ConfirmTargetingAndContinue(), Nice, hit a hero"));
			HitActors.Add(HitActor);
			Handle = StartLocation.MakeTargetDataHandleFromActors(HitActors);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("ConfirmTargetingAndContinue(), Hit nothing"));
			HitResult.Actor = NULL;
			Handle = StartLocation.MakeTargetDataHandleFromHitResult(OwningAbility, HitResult);
		}
		TargetDataReadyDelegate.Broadcast(Handle);
		GetWorld()->GetFirstPlayerController()->CurrentMouseCursor = GetWorld()->GetFirstPlayerController()->DefaultMouseCursor;
		TurnOffTargetHighlight();
	}
}
void AAP_TargetActor_Unit::CancelTargeting()
{
	Super::CancelTargeting();
	GetWorld()->GetFirstPlayerController()->CurrentMouseCursor = GetWorld()->GetFirstPlayerController()->DefaultMouseCursor;
	TurnOffTargetHighlight();
}
FHitResult AAP_TargetActor_Unit::PerformTrace()
{
	//UE_LOG(LogTemp, Warning, TEXT("AAP_TargetActor_Unit::PerformTrace()"));
	// make hit result from cursor
	FHitResult HitResult;
	FVector MouseLoc;
	FVector MouseRot;
	GetWorld()->GetFirstPlayerController()->DeprojectMousePositionToWorld(MouseLoc, MouseRot);;
	const FVector StartTrace = MouseLoc;
	const FVector ShootDir = MouseRot;
	const FVector EndTrace = StartTrace + ShootDir * 20000.0f;
	
	FCollisionQueryParams Params(SCENE_QUERY_STAT(HeroTargetingOverlap), false);
	//GetWorld()->LineTraceSingleByChannel(HitResult, StartTrace, EndTrace, Channel);
	GetWorld()->LineTraceSingleByChannel(HitResult, StartTrace, EndTrace, Channel, Params);
	return HitResult;
}
void AAP_TargetActor_Unit::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//UE_LOG(LogTemp, Warning, TEXT("AAP_TargetActor_Unit::Tick()"));
	if (IsConfirmTargetingAllowed())
	{
		FHitResult HitResult = PerformTrace();
		FVector EndPoint = HitResult.Component.IsValid() ? HitResult.ImpactPoint : HitResult.TraceEnd;

#if ENABLE_DRAW_DEBUG
		if (bDebug)
		{
			//UE_LOG(LogTemp, Warning, TEXT("AAP_TargetActor_Unit::Tick() DrawDebugSphere"));
			if (HitResult.bBlockingHit)
			{
				DrawDebugSphere(GetWorld(), EndPoint, 16, 10, FColor::Green, false);
			}
			else
			{
				DrawDebugSphere(GetWorld(), EndPoint, 16, 10, FColor::Red, false);
			}
		}
#endif // ENABLE_DRAW_DEBUG
		AActor* HitActor = HitResult.Actor.Get();

		auto unit = Cast<IAbilitySystemInterface>(HitActor);
		if (unit)
		{
			//UE_LOG(LogTemp, Warning, TEXT("Tick(), Nice, hit a hero"));
			if (LastUnitHighlighted && LastUnitHighlighted != HitActor)
			{
				//UE_LOG(LogTemp, Warning, TEXT("Tick(), turn off highlight for last hero"));
				//LastUnitHighlighted->GetMesh()->SetRenderCustomDepth(false);
				LastUnitHighlighted = HitActor;
				//UE_LOG(LogTemp, Warning, TEXT("Tick(), turn on highlight for current hero"));
				//unit->GetMesh()->SetRenderCustomDepth(true);
			}
			else
			{
				//UE_LOG(LogTemp, Warning, TEXT("Tick(), turn on highlight for newly found hero"));
				//HitActor->GetMesh()->SetRenderCustomDepth(true);
				LastUnitHighlighted = HitActor;
			}
		}
		else
		{
			//UE_LOG(LogTemp, Warning, TEXT("Tick(), Hit nothing"));
			TurnOffTargetHighlight();
		}
		// update targeting effect here, draw a circle indicating the target location?
	}
}

bool AAP_TargetActor_Unit::ShouldProduceTargetData() const
{
	bool LocallyOwned = true;// TODO: make this true only for local player
	return LocallyOwned || ShouldProduceTargetDataOnServer;
}

void AAP_TargetActor_Unit::TurnOffTargetHighlight()
{
	if (LastUnitHighlighted)
	{
		UE_LOG(LogTemp, Warning, TEXT("TurnOffTargetHighlight(), turn off highlight for last hero"));
		//LastUnitHighlighted->GetMesh()->SetRenderCustomDepth(false);
		LastUnitHighlighted = nullptr;
	}
}
