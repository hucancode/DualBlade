// Fill out your copyright notice in the Description page of Project Settings.

#include "AP_TargetActor_CapsuleAOE.h"
#include "Abilities/GameplayAbility.h"
#include "DrawDebugHelpers.h"

void AAP_TargetActor_CapsuleAOE::BeginPlay()
{
	Super::BeginPlay();
	PrimaryActorTick.TickGroup = TG_PrePhysics;
	bDestroyOnConfirmation = true;
	ShouldProduceTargetDataOnServer = true;
}
void AAP_TargetActor_CapsuleAOE::StartTargeting(UGameplayAbility* Ability)
{
	Super::StartTargeting(Ability);
	SourceActor = Ability->GetCurrentActorInfo()->AvatarActor.Get();
	UE_LOG(LogTemp, Warning, TEXT("AAP_TargetActor_BoxAOE::StartTargeting()"));
	TArray <TWeakObjectPtr<AActor>> HitActors = PerformTrace();
	FGameplayAbilityTargetDataHandle Handle = StartLocation.MakeTargetDataHandleFromActors(HitActors);
	TargetDataReadyDelegate.Broadcast(Handle);
	Destroy();
}

void AAP_TargetActor_CapsuleAOE::ConfirmTargetingAndContinue()
{
	check(ShouldProduceTargetData());
	if (SourceActor)
	{
		TArray <TWeakObjectPtr<AActor>> HitActors = PerformTrace();
		FGameplayAbilityTargetDataHandle Handle = StartLocation.MakeTargetDataHandleFromActors(HitActors);
		TargetDataReadyDelegate.Broadcast(Handle);
	}
}
TArray<TWeakObjectPtr<AActor>> AAP_TargetActor_CapsuleAOE::PerformTrace()
{
	FCollisionObjectQueryParams ObjectToScan = FCollisionObjectQueryParams(Channel);
	FCollisionQueryParams Params(SCENE_QUERY_STAT(CapsuleTargetingOverlap), false);
	Params.bReturnPhysicalMaterial = false;
	//Params.bTraceAsyncScene = false;
	TArray<FOverlapResult> Overlaps;
	TArray<TWeakObjectPtr<AActor>>	HitActors;
	const float HalfHeight = 300.0f;
	FCollisionShape Shape = FCollisionShape::MakeCapsule(Radius, HalfHeight);
	GetWorld()->OverlapMultiByObjectType(Overlaps, Location, FQuat::Identity, ObjectToScan, Shape, Params);
#if ENABLE_DRAW_DEBUG
	if (bDebug)
	{
		DrawDebugCapsule(GetWorld(), Location, HalfHeight, Radius, FQuat::Identity, FColor::Green, false, 1.5f);
	}
#endif
	for (int32 i = 0; i < Overlaps.Num(); ++i)
	{
		//Should this check to see if these pawns are in the AimTarget list?
		APawn* PawnActor = Cast<APawn>(Overlaps[i].GetActor());
		if (PawnActor && !HitActors.Contains(PawnActor) && Filter.FilterPassesForActor(PawnActor))
		{
			HitActors.Add(PawnActor);
		}
	}
	UE_LOG(LogTemp, Warning, TEXT("AAP_TargetActor_BoxAOE::PerformTrace() trace finished, hit %d targets"), HitActors.Num());
	return HitActors;
}
