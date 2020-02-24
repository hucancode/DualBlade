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
}

void AAP_TargetActor_CapsuleAOE::ConfirmTargetingAndContinue()
{
	check(ShouldProduceTargetData());
	if (SourceActor)
	{
		auto hit_actors = PerformTrace();
		FGameplayAbilityTargetDataHandle Handle = StartLocation.MakeTargetDataHandleFromActors(hit_actors);
		TargetDataReadyDelegate.Broadcast(Handle);
	}
}
TArray<TWeakObjectPtr<AActor>> AAP_TargetActor_CapsuleAOE::PerformTrace()
{
	auto object_to_scan = FCollisionObjectQueryParams(Channel);
	auto params = FCollisionQueryParams(SCENE_QUERY_STAT(CapsuleTargetingOverlap), false);
	params.bReturnPhysicalMaterial = false;
	//params.bTraceAsyncScene = false;
	auto location = StartLocation.GetTargetingTransform().GetLocation();
	TArray<FOverlapResult> overlaps;
	TArray<TWeakObjectPtr<AActor>>	hit_actors;
	auto shape = FCollisionShape::MakeCapsule(Radius, TARGET_CAPSULE_AOE_HALF_HEIGHT);
	SourceActor->GetWorld()->OverlapMultiByObjectType(overlaps, location, FQuat::Identity, object_to_scan, shape, params);
#if ENABLE_DRAW_DEBUG
	if (bDebug)
	{
		DrawDebugCapsule(SourceActor->GetWorld(), location, TARGET_CAPSULE_AOE_HALF_HEIGHT, Radius, FQuat::Identity, FColor::Green, false, 1.5f);
	}
#endif
	for (int32 i = 0; i < overlaps.Num(); ++i)
	{
		//Should this check to see if these pawns are in the AimTarget list?
		auto pawn = Cast<APawn>(overlaps[i].GetActor());
		if (pawn && !hit_actors.Contains(pawn) && Filter.FilterPassesForActor(pawn))
		{
			hit_actors.Add(pawn);
		}
	}
	UE_LOG(LogTemp, Warning, TEXT("AAP_TargetActor_BoxAOE::PerformTrace() trace finished, hit %d targets"), hit_actors.Num());
	return hit_actors;
}
