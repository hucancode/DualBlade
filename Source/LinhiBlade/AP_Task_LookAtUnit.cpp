// Fill out your copyright notice in the Description page of Project Settings.

#include "AP_Task_LookAtUnit.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AIController.h"
#include "Runtime/Engine/Classes/Kismet/KismetMathLibrary.h"
#include <GameplayAbilities\Public\AbilitySystemBlueprintLibrary.h>


UAP_Task_LookAtUnit* UAP_Task_LookAtUnit::LookAtUnit(UGameplayAbility* OwningAbility, FName TaskInstanceName, 
	AActor* Unit)
{
	UAP_Task_LookAtUnit* MyObj = NewAbilityTask<UAP_Task_LookAtUnit>(OwningAbility, TaskInstanceName);
	MyObj->TargetUnit = Unit;
	UE_LOG(LogTemp, Warning, TEXT("UAP_Task_MoveNearLocation::LookAtUnit() %s"), MyObj->TargetUnit ? *MyObj->TargetUnit->GetName() : TEXT("NULL"));
	return MyObj;
}

UAP_Task_LookAtUnit* UAP_Task_LookAtUnit::LookAtUnitTarget(UGameplayAbility* OwningAbility, FName TaskInstanceName, 
	FGameplayAbilityTargetDataHandle TargetHandle)
{
	UAP_Task_LookAtUnit* MyObj = NewAbilityTask<UAP_Task_LookAtUnit>(OwningAbility, TaskInstanceName);
	TArray<AActor*> Actors = UAbilitySystemBlueprintLibrary::GetActorsFromTargetData(TargetHandle, 0);
	MyObj->TargetUnit = Actors.Num() > 0 ? Actors.Last() : 0;
	UE_LOG(LogTemp, Warning, TEXT("UAP_Task_MoveNearLocation::LookAtUnit() %s"), MyObj->TargetUnit ? *MyObj->TargetUnit->GetName() : TEXT("NULL"));
	return MyObj;
}

UAP_Task_LookAtUnit::UAP_Task_LookAtUnit()
{
	Reached = false;
	bTickingTask = true;
}

UAP_Task_LookAtUnit::~UAP_Task_LookAtUnit()
{
}

void UAP_Task_LookAtUnit::Activate()
{
	if(!TargetUnit)
	{
		UE_LOG(LogTemp, Warning, TEXT("UAP_Task_LookAtUnit::Activate() invalid target"));
		Reached = true;
		OnRotationCancelled.Broadcast();
	}
	ACharacter* Character = Cast<ACharacter>(GetAvatarActor());
	if (Character)
	{
		TurnRate = Character->GetCharacterMovement()->RotationRate.Yaw;
	}
	else
	{
		Reached = true;
		OnRotationCancelled.Broadcast();
	}
}

void UAP_Task_LookAtUnit::OnDestroy(bool AbilityIsEnding)
{
}

void UAP_Task_LookAtUnit::TickTask(float DeltaTime)
{
	if (!Reached)
	{
		ACharacter* Character = Cast<ACharacter>(GetAvatarActor());
		//check(Character);
		if (!Character) return;
		AAIController* Controller = Character->GetController<AAIController>();
		//check(Controller);
		if (!Controller) return;
		EPathFollowingStatus::Type MoveStatus = Controller->GetMoveStatus();
		if (MoveStatus != EPathFollowingStatus::Idle)
		{
			UE_LOG(LogTemp, Warning, TEXT("UAP_Task_MoveNearLocation::TickTask() rotation cancelled, GetMoveStatus = %d"), MoveStatus);
			Reached = true;
			OnRotationCancelled.Broadcast();
		}
		else
		{
			Destination = UKismetMathLibrary::FindLookAtRotation(GetAvatarActor()->GetActorLocation(), 
				TargetUnit->GetActorLocation());
			Destination.Pitch = 0.0f;
			Destination.Roll = 0.0f;
			FRotator Source = GetAvatarActor()->GetActorRotation();
			FRotator Delta = Destination - Source;
			if (UKismetMathLibrary::Abs(Delta.Yaw) > 180.0f)
			{
				Delta.Yaw = 180.0f - Delta.Yaw;
			}
			float MicroTurn = TurnRate * DeltaTime;
			if (UKismetMathLibrary::Abs(Delta.Yaw) < MicroTurn)
			{
				UE_LOG(LogTemp, Warning, TEXT("UAP_Task_MoveNearLocation::TickTask() rotation finished"));
				GetAvatarActor()->SetActorRotation(Destination);
				Reached = true;
				OnTargetRotationReached.Broadcast();
			}
			else
			{
				Source.Yaw += MicroTurn * UKismetMathLibrary::SignOfFloat(Delta.Yaw);
				GetAvatarActor()->SetActorRotation(Source);
			}
		}
	}
}
