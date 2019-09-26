// Fill out your copyright notice in the Description page of Project Settings.

#include "AP_Task_MoveNearUnit.h"
#include "GameFramework/Character.h"
#include <GameplayAbilities\Public\AbilitySystemBlueprintLibrary.h>

UAP_Task_MoveNearUnit * UAP_Task_MoveNearUnit::MoveNearUnit(UGameplayAbility * OwningAbility, FName TaskInstanceName, 
	AActor* Unit, float Range)
{
	// receive call from blueprint
	UAP_Task_MoveNearUnit* MyObj = NewAbilityTask<UAP_Task_MoveNearUnit>(OwningAbility, TaskInstanceName);
	MyObj->TargetUnit = Unit;
	MyObj->AcceptanceRadius = Range;
	UE_LOG(LogTemp, Warning, TEXT("UAP_Task_MoveNearUnit::MoveNearUnit() %s"), MyObj->TargetUnit ? *MyObj->TargetUnit->GetName() : TEXT("NULL"));
	return MyObj;
}

UAP_Task_MoveNearUnit* UAP_Task_MoveNearUnit::MoveNearUnitTarget(UGameplayAbility* OwningAbility, FName TaskInstanceName, 
	FGameplayAbilityTargetDataHandle TargetHandle, float Range)
{
	// receive call from blueprint
	UAP_Task_MoveNearUnit* MyObj = NewAbilityTask<UAP_Task_MoveNearUnit>(OwningAbility, TaskInstanceName);
	TArray<AActor*> Actors = UAbilitySystemBlueprintLibrary::GetActorsFromTargetData(TargetHandle, 0);
	MyObj->TargetUnit = Actors.Num() > 0 ? Actors.Last() : 0;
	MyObj->AcceptanceRadius = Range;
	UE_LOG(LogTemp, Warning, TEXT("UAP_Task_MoveNearUnit::MoveNearUnit() %s"), MyObj->TargetUnit ? *MyObj->TargetUnit->GetName() : TEXT("NULL"));
	return MyObj;
}

UAP_Task_MoveNearUnit::UAP_Task_MoveNearUnit()
{
	OnMovementCompletedDelegate.BindUFunction(this, "OnMovementCompleted");
	AcceptanceRadius = -1.0f;
}

UAP_Task_MoveNearUnit::~UAP_Task_MoveNearUnit()
{
	OnMovementCompletedDelegate.Unbind();
}

void UAP_Task_MoveNearUnit::Activate()
{
	UE_LOG(LogTemp, Warning, TEXT("UAP_Task_MoveNearUnit::Activate()"));
	if (!TargetUnit)
	{
		UE_LOG(LogTemp, Warning, TEXT("UAP_Task_MoveNearUnit::Activate() invalid target, return"));
		OnMovementCancelled.Broadcast();
		return;
	}
	
	ACharacter* Character = Cast<ACharacter>(GetAvatarActor());
	//check(Character);
	if (!Character) return;
	AAIController* Controller = Character->GetController<AAIController>();
	//check(Controller);
	if (!Controller) return;
	EPathFollowingRequestResult::Type RequestResult = Controller->MoveToActor(TargetUnit, AcceptanceRadius);
	switch (RequestResult)
	{
	case EPathFollowingRequestResult::Failed:
		UE_LOG(LogTemp, Warning, TEXT("UAP_Task_MoveNearUnit::Activate() request = Failed"));
		OnMovementCancelled.Broadcast();
		break;
	case EPathFollowingRequestResult::AlreadyAtGoal:
		UE_LOG(LogTemp, Warning, TEXT("UAP_Task_MoveNearUnit::Activate() request = AlreadyAtGoal"));
		OnTargetLocationReached.Broadcast();
		break;
	case EPathFollowingRequestResult::RequestSuccessful:
		UE_LOG(LogTemp, Warning, TEXT("UAP_Task_MoveNearUnit::Activate() request = RequestSuccessful"));
		Controller->ReceiveMoveCompleted.Add(OnMovementCompletedDelegate);
		break;
	default:
		UE_LOG(LogTemp, Warning, TEXT("UAP_Task_MoveNearUnit::Activate() request = unknown"));
		break;
	}
}

void UAP_Task_MoveNearUnit::OnDestroy(bool AbilityIsEnding)
{
	UE_LOG(LogTemp, Warning, TEXT("UAP_Task_MoveNearUnit::OnDestroy() AbilityIsEnding = %d"), AbilityIsEnding);
	ACharacter* Character = Cast<ACharacter>(GetAvatarActor());
	//check(Character);
	if (!Character) return;
	AAIController* Controller = Character->GetController<AAIController>();
	//check(Controller);
	if (!Controller) return;
	Controller->ReceiveMoveCompleted.Remove(OnMovementCompletedDelegate);
}

void UAP_Task_MoveNearUnit::OnMovementCompleted(FAIRequestID RequestID, EPathFollowingResult::Type Result)
{
	UE_LOG(LogTemp, Warning, TEXT("UAP_Task_MoveNearUnit::OnMovementCompleted()"));
	ACharacter* Character = Cast<ACharacter>(GetAvatarActor());
	//check(Character);
	if (!Character) return;
	AAIController* Controller = Character->GetController<AAIController>();
	//check(Controller);
	if (!Controller) return;
	// broadcast result back to blueprint
	switch (Result)
	{
	case EPathFollowingResult::Type::Success:
		// success
		OnTargetLocationReached.Broadcast();
		Controller->ReceiveMoveCompleted.Remove(OnMovementCompletedDelegate);
		UE_LOG(LogTemp, Warning, TEXT("UAP_Task_MoveNearUnit::OnMovementCompleted() Success"));
		break;
	case EPathFollowingResult::Type::Blocked:
	case EPathFollowingResult::Type::OffPath:
		// do it again
		UE_LOG(LogTemp, Warning, TEXT("UAP_Task_MoveNearUnit::OnMovementCompleted() Blocked/OffPath"));
		Controller->MoveToActor(TargetUnit, AcceptanceRadius);
		break;
	case EPathFollowingResult::Type::Aborted:
	case EPathFollowingResult::Type::Invalid:
		// cancelled or something?
		UE_LOG(LogTemp, Warning, TEXT("UAP_Task_MoveNearUnit::OnMovementCompleted() Aborted/Invalid"));
		OnMovementCancelled.Broadcast();
		Controller->ReceiveMoveCompleted.Remove(OnMovementCompletedDelegate);
		break;
	}
}
