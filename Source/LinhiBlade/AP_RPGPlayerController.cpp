// Fill out your copyright notice in the Description page of Project Settings.


#include "AP_RPGPlayerController.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AbilitySystemGlobals.h"

#ifndef RAY_LENGTH
#define RAY_LENGTH 999999.0f
#endif

#ifndef UE_LOG_FAST
#define UE_LOG_FAST(Format, ...) UE_LOG(LogTemp, Display, Format, ##__VA_ARGS__)
#endif
AAP_RPGPlayerController::AAP_RPGPlayerController()
{
	Team = EGameTeam::Neutral;
	TeamTag = FGameplayTag::RequestGameplayTag("Combat.Team_Neutral");
}
void AAP_RPGPlayerController::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG_FAST(TEXT("AAP_RPGPlayerController::BeginPlay(), I am %s my netmode is %d"), *GetName(), GetNetMode());
	//TODO: find a better place to init global data
	if (!UAbilitySystemGlobals::Get().IsAbilitySystemGlobalsInitialized())
	{
		UAbilitySystemGlobals::Get().InitGlobalData();
	}
}
ETeamAttitude::Type AAP_RPGPlayerController::GetAttituteTowardPlayer(AAP_RPGPlayerController* Other)
{
	return FGenericTeamId::GetAttitude((uint8)Other->Team, (uint8)this->Team);
}

void AAP_RPGPlayerController::SetTeam(EGameTeam NewTeam)
{
	Team = NewTeam;
	TeamTag = Team == EGameTeam::Team1 ? FGameplayTag::RequestGameplayTag("Combat.Team_1") :
		Team == EGameTeam::Team2 ? FGameplayTag::RequestGameplayTag("Combat.Team_2") :
		FGameplayTag::RequestGameplayTag("Combat.Team_Neutral");
}

bool AAP_RPGPlayerController::LineTraceGround(FVector Start, FVector Direction, FVector& OutLocation)
{
	FHitResult hit_result;
	FVector start = Start;
	FVector end = Start + Direction * RAY_LENGTH;
	bool hit = GetWorld()->LineTraceSingleByChannel(hit_result, start, end, GroundChannel);
	OutLocation = hit_result.Location;
#ifdef ENABLE_DRAW_DEBUG
	//DrawDebugSphere(GetWorld(), OutLocation, 16, 10, FColor::Green, false);
#endif // ENABLE_DRAW_DEBUG
	return hit;
}

void AAP_RPGPlayerController::PlayerFaceTo(FVector Target)
{
	if (!GetPawn())
	{
		return;
	}
	auto rotator = UKismetMathLibrary::FindLookAtRotation(GetPawn()->GetActorLocation(),Target);
	SetControlRotation(rotator);
}

void AAP_RPGPlayerController::PlayerFaceToMouse()
{
	auto pawn = GetPawn();
	if (!pawn)
	{
		return;
	}
	auto component = Cast<UCharacterMovementComponent>(pawn->GetComponentByClass(UCharacterMovementComponent::StaticClass()));
	if (component)
	{
		if (component->bOrientRotationToMovement)
		{
			return;
		}
	}
	FVector pos, dir;
	bool ret;
	ret = DeprojectMousePositionToWorld(pos, dir);
	if (!ret)
	{
		return;
	}
	FVector location;
	ret = LineTraceGround(pos, dir, location);
	if (!ret)
	{
		return;
	}
	PlayerFaceTo(location);
}
