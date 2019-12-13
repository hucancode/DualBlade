// Fill out your copyright notice in the Description page of Project Settings.


#include "AP_AbilityBase.h"

UAP_AbilityBase::UAP_AbilityBase()
{

}
UAP_AbilityBase::~UAP_AbilityBase()
{

}

ECollisionChannel UAP_AbilityBase::GetTraceChannel()
{
	// Those channel are subject to change
	// See DefaultEngine.ini for lastest update
	auto ret = ECollisionChannel();
	switch (TargetingPolicy)
	{
	case ETargetingPolicy::None:
		break;
	case ETargetingPolicy::Ground:
		ret = ECollisionChannel::ECC_GameTraceChannel3; //Floor
		break;
	case ETargetingPolicy::UnitAll:
		ret = ECollisionChannel::ECC_GameTraceChannel11; //UnitAll
		break;
	case ETargetingPolicy::UnitAlly:
		// need to calculate this on run time
		ret = ECollisionChannel::ECC_GameTraceChannel8; //Unit_Team1
		break;
	case ETargetingPolicy::UnitAllyExcludeSelf:
		// need to calculate this on run time
		ret = ECollisionChannel::ECC_GameTraceChannel8; //Unit_Team1
		break;
	case ETargetingPolicy::UnitEnemy:
		// need to calculate this on run time
		ret = ECollisionChannel::ECC_GameTraceChannel9; //Unit_Team2
		break;
	default:
		break;
	}
	return ret;
}

bool UAP_AbilityBase::LineTraceUnit(FVector Start, FVector Direction, AActor*& OutActor)
{
	if (TargetingPolicy < ETargetingPolicy::UnitAll)
	{
		return false;
	}
	FHitResult HitResult;
	FVector End = Start + Direction * RAY_LENGTH;
	ECollisionChannel Channel = GetTraceChannel();
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(GetOwningActorFromActorInfo());
	bool hit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, Channel, Params);
	OutActor = HitResult.GetActor();
	return hit;
}

bool UAP_AbilityBase::LineTraceGround(FVector Start, FVector Direction, FVector& OutLocation)
{
	if (TargetingPolicy != ETargetingPolicy::Ground)
	{
		return false;
	}
	FHitResult HitResult;
	FVector End = Start + Direction * RAY_LENGTH;
	ECollisionChannel Channel = GetTraceChannel();
	FCollisionQueryParams Params;
	bool hit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, Channel, Params);
	OutLocation = HitResult.Location;
	return hit;
}
