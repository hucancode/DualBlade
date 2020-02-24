// Fill out your copyright notice in the Description page of Project Settings.

#include "AP_TargetActor_BoxAOE.h"
#include "Abilities/GameplayAbility.h"
#include "Kismet/KismetMathLibrary.h"
#include "DrawDebugHelpers.h"

void AAP_TargetActor_BoxAOE::BeginPlay()
{
	Super::BeginPlay();
	PrimaryActorTick.TickGroup = TG_PrePhysics;
	bDestroyOnConfirmation = true;
	ShouldProduceTargetDataOnServer = true;
}
void AAP_TargetActor_BoxAOE::StartTargeting(UGameplayAbility* Ability)
{
	Super::StartTargeting(Ability);
	SourceActor = Ability->GetCurrentActorInfo()->AvatarActor.Get();
	UE_LOG(LogTemp, Warning, TEXT("AAP_TargetActor_BoxAOE::StartTargeting()"));
}

void AAP_TargetActor_BoxAOE::ConfirmTargetingAndContinue()
{
	check(ShouldProduceTargetData());
	if (SourceActor)
	{
		auto hit_actors = PerformTrace();
		FGameplayAbilityTargetDataHandle Handle = StartLocation.MakeTargetDataHandleFromActors(hit_actors);
		TargetDataReadyDelegate.Broadcast(Handle);
	}
}

TArray<TWeakObjectPtr<AActor>> AAP_TargetActor_BoxAOE::PerformTrace()
{
	auto object_to_scan = FCollisionObjectQueryParams(Channel);
	auto params = FCollisionQueryParams(SCENE_QUERY_STAT(BoxTargetingOverlap), false);
	params.bReturnPhysicalMaterial = false;
	//params.bTraceAsyncScene = false;
	auto location = StartLocation.GetTargetingTransform().GetLocation();
	auto rotation = FRotator(0.0f, BoxRotation, 0.0f);
	TArray<FOverlapResult> overlaps;
	TArray<TWeakObjectPtr<AActor>>	hit_actors;
	auto shape = FCollisionShape::MakeBox(Size);
#if ENABLE_DRAW_DEBUG
	if (bDebug)
	{
		DrawDebugBox(SourceActor->GetWorld(), location, Size, rotation.Quaternion(), FColor::Green, false, 1.5f);
	}
#endif
	SourceActor->GetWorld()->OverlapMultiByObjectType(overlaps, location, rotation.Quaternion(), object_to_scan, shape, params);
	for (int32 i = 0; i < overlaps.Num(); ++i)
	{
		auto pawn = Cast<APawn>(overlaps[i].GetActor());
		if (pawn && !hit_actors.Contains(pawn) && Filter.FilterPassesForActor(pawn))
		{
			hit_actors.Add(pawn);
		}
	}
	UE_LOG(LogTemp, Warning, TEXT("AAP_TargetActor_BoxAOE::PerformTrace() trace finished, hit %d targets"), hit_actors.Num());
	return hit_actors;
}
