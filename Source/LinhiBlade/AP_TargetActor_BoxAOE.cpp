// Fill out your copyright notice in the Description page of Project Settings.

#include "AP_TargetActor_BoxAOE.h"
#include "Runtime/Engine/Classes/Kismet/KismetMathLibrary.h"
#include "DrawDebugHelpers.h"

void AAP_TargetActor_BoxAOE::BeginPlay()
{
	Super::BeginPlay();
	bDestroyOnConfirmation = true;
}
void AAP_TargetActor_BoxAOE::StartTargeting(UGameplayAbility* Ability)
{
	Super::StartTargeting(Ability);
	UE_LOG(LogTemp, Warning, TEXT("AAP_TargetActor_BoxAOE::StartTargeting()"));
	TArray <TWeakObjectPtr<AActor>> HitActors = PerformTrace();
	FGameplayAbilityTargetDataHandle Handle = StartLocation.MakeTargetDataHandleFromActors(HitActors);
	TargetDataReadyDelegate.Broadcast(Handle);
	Destroy();
}

bool AAP_TargetActor_BoxAOE::ShouldProduceTargetData() const
{
	bool LocallyOwned = true;// TODO: make this true only for local player
	return LocallyOwned || ShouldProduceTargetDataOnServer;
}

TArray<TWeakObjectPtr<AActor>> AAP_TargetActor_BoxAOE::PerformTrace()
{
	FCollisionObjectQueryParams ObjectToScan = FCollisionObjectQueryParams(Channel);
	FCollisionQueryParams Params(SCENE_QUERY_STAT(BoxTargetingOverlap), false);
	Params.bReturnPhysicalMaterial = false;
	//Params.bTraceAsyncScene = false;
	FRotator Rotation = UKismetMathLibrary::FindLookAtRotation(
		StartLocation.GetTargetingTransform().GetTranslation(), 
		Location);
	Rotation.Pitch = 0.0f;
	Rotation.Roll = 0.0f;
	TArray<FOverlapResult> Overlaps;
	TArray<TWeakObjectPtr<AActor>>	HitActors;
	FCollisionShape Shape = FCollisionShape::MakeBox(Size);
#if ENABLE_DRAW_DEBUG
	if (bDebug)
	{
		DrawDebugBox(GetWorld(), Location, Size, Rotation.Quaternion(), FColor::Green, false, 1.5f);
	}
#endif
	bool ret = GetWorld()->OverlapMultiByObjectType(Overlaps, Location, Rotation.Quaternion(), ObjectToScan, Shape, Params);
	if (!ret)
	{
		UE_LOG(LogTemp, Warning, TEXT("AAP_TargetActor_BoxAOE::PerformTrace() hit nothing"));
		return HitActors;
	}
	for (int32 i = 0; i < Overlaps.Num(); ++i)
	{
		APawn* PawnActor = Cast<APawn>(Overlaps[i].GetActor());
		if (PawnActor && !HitActors.Contains(PawnActor) && Filter.FilterPassesForActor(PawnActor))
		{
			HitActors.Add(PawnActor);
		}
	}
	UE_LOG(LogTemp, Warning, TEXT("AAP_TargetActor_BoxAOE::PerformTrace() trace finished, hit %d targets"), HitActors.Num());
	return HitActors;
}
