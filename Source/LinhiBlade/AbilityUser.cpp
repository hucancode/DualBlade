// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilityUser.h"
#include "AP_FightUnit.h"
#include "Net/UnrealNetwork.h"
#include "AbilitySystemGlobals.h"
#include "AbilitySystemBlueprintLibrary.h"

#ifndef UE_LOG_FAST
#define UE_LOG_FAST(Format, ...) UE_LOG(LogTemp, Display, Format, ##__VA_ARGS__)
#endif

// Sets default values for this component's properties
UAbilityUser::UAbilityUser()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	bWantsInitializeComponent = true;
	AbilitySystem = nullptr;
	// ...
}
void UAbilityUser::InitializeComponent()
{
	Super::InitializeComponent();
	auto owner = Cast<IAbilitySystemInterface>(GetOwner());
	check(owner);
	AbilitySystem = owner->GetAbilitySystemComponent();
	check(AbilitySystem);
	AbilitySystem->AbilityCommittedCallbacks.AddUObject(this, &UAbilityUser::HandleAbilityCommitted);
	AbilitySystem->AbilityEndedCallbacks.AddUObject(this, &UAbilityUser::HandleAbilityEnded);
	AbilitySystem->AbilityActivatedCallbacks.AddUObject(this, &UAbilityUser::HandleAbilityActivated);
	AbilitySystem->OnGameplayEffectAppliedDelegateToSelf.AddUObject(this, &UAbilityUser::HandleEffectApplied);
}

// Called when the game starts
void UAbilityUser::BeginPlay()
{
	Super::BeginPlay();
	RecheckAbilitySlot();
	RecheckAbilityState();
}


// Called every frame
void UAbilityUser::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UAbilityUser::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UAbilityUser, AbilityStates);
	DOREPLIFETIME(UAbilityUser, AbilityBehaviors);
	DOREPLIFETIME(UAbilityUser, AbilityHandles);
}

void UAbilityUser::GiveAbility(TSubclassOf<UAP_AbilityBase> Ability)
{
	if (!GetOwner()->HasAuthority())
	{
		return;
	}
	FGameplayAbilitySpec* spec;
	FGameplayAbilitySpecHandle handle;
	spec = AbilitySystem->FindAbilitySpecFromClass(Ability);
	if (!spec)
	{
		handle = AbilitySystem->GiveAbility(FGameplayAbilitySpec(Ability, 0, INDEX_NONE, this));
		spec = AbilitySystem->FindAbilitySpecFromHandle(handle);
	}
	else
	{
		handle = spec->Handle;
	}
	if (AbilityHandles.Find(handle) != INDEX_NONE)
	{
		UE_LOG_FAST(TEXT("give ability fail, already have this ability"));
		return;
	}
	
	AbilityHandles.Add(handle);
	AbilityStates.AddDefaulted();
	UE_LOG_FAST(TEXT("give ability %d"), AbilityHandles.Num());
	RecheckAbilitySlot();
	RecheckAbilityState();
}

void UAbilityUser::RemoveAbility(TSubclassOf<UAP_AbilityBase> Ability)
{
	if (!GetOwner()->HasAuthority())
	{
		return;
	}
	auto spec = AbilitySystem->FindAbilitySpecFromClass(Ability);
	int index = AbilityHandles.Find(spec->Handle);
	if (index == INDEX_NONE)
	{
		return;
	}
	AbilityHandles.RemoveAt(index);
	AbilityStates.RemoveAt(index);
	RecheckAbilitySlot();
	RecheckAbilityState();
}

void UAbilityUser::HandleEffectApplied(UAbilitySystemComponent* Source, const FGameplayEffectSpec& Spec, FActiveGameplayEffectHandle Handle)
{
	UE_LOG_FAST(TEXT("AAP_Hero::OnGameplayEffectAppliedToSelf duration = %f"), Spec.GetDuration());
	OnEffectApplied.Broadcast(Source, Spec, Handle);
	FOnActiveGameplayEffectRemoved_Info* Delegate = AbilitySystem->OnGameplayEffectRemoved_InfoDelegate(Handle);
	if (Delegate)
	{
		Delegate->AddUObject(this, &UAbilityUser::HandleEffectRemoved);
	}
}

void UAbilityUser::HandleEffectRemoved(const FGameplayEffectRemovalInfo& Info)
{
	OnEffectRemoved.Broadcast(Info);
}

void UAbilityUser::HandleAbilityActivated(UGameplayAbility* Ability)
{
	UE_LOG_FAST(TEXT("AAP_Hero::OnAbilityActivated %s"), *Ability->GetFName().ToString());
	FGameplayAbilitySpecHandle Handle = Ability->GetCurrentAbilitySpecHandle();
	int Index = AbilityHandles.Find(Handle);
	if (Index == INDEX_NONE)
	{
		return;
	}
	AbilityStates[Index] = EAbilityState::Casting;
	RecheckAbilityState();
}

void UAbilityUser::HandleAbilityCommitted(UGameplayAbility* Ability)
{
	UE_LOG_FAST(TEXT("AAP_Hero::OnAbilityCommitted %s"), *Ability->GetFName().ToString());
	FGameplayAbilitySpecHandle Handle = Ability->GetCurrentAbilitySpecHandle();
	int Index = AbilityHandles.Find(Handle);
	if (Index == INDEX_NONE)
	{
		return;
	}
	if (Ability->CheckCooldown(Handle, Ability->GetCurrentActorInfo()))
	{
		return;
	}
	FGameplayEffectQuery query;
	query.EffectDefinition = Ability->GetCooldownGameplayEffect()->GetClass();
	TArray<FActiveGameplayEffectHandle> Handles = AbilitySystem->GetActiveEffects(query);
	UE_LOG_FAST(TEXT("setting up delegate..."));
	if (!Handles.Num())
	{
		UE_LOG_FAST(TEXT("can't find cooldown effect, setup unsuccessful"));
		AbilityStates[Index] = EAbilityState::Ready;
		RecheckAbilityState();
		return;
	}
	AbilityStates[Index] = EAbilityState::OnCooldown;
	RecheckAbilityState();
	FActiveGameplayEffectHandle CooldownEffect = Handles.Last();
	FOnActiveGameplayEffectRemoved_Info* Delegate = AbilitySystem->OnGameplayEffectRemoved_InfoDelegate(CooldownEffect);
	if (!Delegate)
	{
		UE_LOG_FAST(TEXT("can't find delegate, setup unsuccessful"));
		return;
	}
	Delegate->AddUObject(this, &UAbilityUser::HandleAbilityOffCooldown);
	UE_LOG_FAST(TEXT("delegate setup successful"));
}

void UAbilityUser::HandleAbilityEnded(UGameplayAbility* Ability)
{
	UE_LOG_FAST(TEXT("AAP_Hero::OnAbilityEnded %s"), *Ability->GetFName().ToString());
	auto spec = AbilitySystem->FindAbilitySpecFromClass(Ability->GetClass());
	if (!spec)
	{
		return;
	}
	auto handle = spec->Handle;
	int index = AbilityHandles.IndexOfByKey(handle);
	if (index == INDEX_NONE)
	{
		return;
	}
	if (AbilityStates[index] == EAbilityState::Casting)
	{
		AbilityStates[index] = EAbilityState::Ready;
		RecheckAbilityState();
	}
}

void UAbilityUser::HandleAbilityOffCooldown(const FGameplayEffectRemovalInfo& InGameplayEffectRemovalInfo)
{
	UE_LOG_FAST(TEXT("AAP_Hero::OnAbilityOffCooldown"));
	auto ability = InGameplayEffectRemovalInfo.EffectContext.GetAbility();
	auto spec = AbilitySystem->FindAbilitySpecFromClass(ability->GetClass());
	auto handle = spec->Handle;
	int index = AbilityHandles.IndexOfByKey(handle);
	if (index == INDEX_NONE)
	{
		return;
	}
	AbilityStates[index] = EAbilityState::Ready;
	RecheckAbilityState();
}

int UAbilityUser::GetAbilityCount() const
{
	return AbilityHandles.Num();
}

UAP_AbilityBase* UAbilityUser::GetAbility(int AbilitySlot) const
{
	bool valid = AbilitySystem && AbilityHandles.IsValidIndex(AbilitySlot);
	if (!valid)
	{
		UE_LOG_FAST(TEXT("get ability fail because invalid ability system"));
		return nullptr;
	}
	auto handle = AbilityHandles[AbilitySlot];
	auto spec = AbilitySystem->FindAbilitySpecFromHandle(handle);
	if (!spec)
	{
		UE_LOG_FAST(TEXT("get ability fail because invalid spec"));
		return nullptr;
	}
	auto ability = Cast<UAP_AbilityBase>(spec->Ability);
	return ability;
}

bool UAbilityUser::CanActivateAbility(int AbilitySlot) const
{
	if (!AbilityStates.IsValidIndex(AbilitySlot))
	{
		return false;
	}
	if (AbilityStates[AbilitySlot] != EAbilityState::Ready && 
		AbilityStates[AbilitySlot] != EAbilityState::Casting)
	{
		return false;
	}
	auto handle = AbilityHandles[AbilitySlot];
	auto spec = AbilitySystem->FindAbilitySpecFromHandle(handle);
	if (!spec)
	{
		return false;
	}
	if (spec->Level <= 0)
	{
		//return false;
	}
	FGameplayTagContainer source_tag;
	AbilitySystem->GetOwnedGameplayTags(source_tag);
	auto actor_info = AbilitySystem->AbilityActorInfo.Get();
	auto ability = spec->Ability;
	auto ability_instanced = spec->GetPrimaryInstance();
	auto ability_source = ability_instanced ? ability_instanced : ability;
	if (!ability_source)
	{
		return false;
	}
	bool ret = ability_source->CanActivateAbility(handle, actor_info, &source_tag);
	return ret;
}

bool UAbilityUser::LevelUpAbility(int AbilitySlot)
{
	if (!GetOwner()->HasAuthority())
	{
		return false;
	}
	bool valid = AbilitySystem && AbilityHandles.IsValidIndex(AbilitySlot);
	if (!valid)
	{
		return false;
	}
	auto handle = AbilityHandles[AbilitySlot];
	auto spec = AbilitySystem->FindAbilitySpecFromHandle(handle);
	if (!spec)
	{
		return false;
	}
	spec->Level += 1;
	AbilitySystem->MarkAbilitySpecDirty(*spec);
	RecheckAbilityLevel(AbilitySlot);
	return true;
}

int UAbilityUser::ResetAbilityLevel(int AbilitySlot)
{
	bool valid = AbilitySystem && AbilityHandles.IsValidIndex(AbilitySlot);
	if (!valid)
	{
		return 0;
	}
	auto handle = AbilityHandles[AbilitySlot];
	auto spec = AbilitySystem->FindAbilitySpecFromHandle(handle);
	if (!spec)
	{
		return 0;
	}
	int ret = spec->Level;
	spec->Level = 0;
	RecheckAbilityLevel(AbilitySlot);
	return ret;
}

float UAbilityUser::GetAbilityCooldown(int AbilitySlot)
{
	bool valid = AbilitySystem && AbilityHandles.IsValidIndex(AbilitySlot);
	UE_LOG_FAST(TEXT("about to check ability cooldown %d, valid=%d"), AbilitySlot, valid);
	float ret = 0.0f;
	if (!valid)
	{
		UE_LOG_FAST(TEXT("valid false return %f"), ret);
		return ret;
	}
	auto handle = AbilityHandles[AbilitySlot];
	auto spec = AbilitySystem->FindAbilitySpecFromHandle(handle);
	if (!spec)
	{
		return ret;
	}
	auto ability = spec->Ability;
	ret = ability->GetCooldownTimeRemaining();
	UE_LOG_FAST(TEXT("return %f"), ret);
	return ret;
}

float UAbilityUser::GetAbilityCooldownPercent(int AbilitySlot)
{
	bool valid = AbilitySystem && AbilityHandles.IsValidIndex(AbilitySlot);
	UE_LOG_FAST(TEXT("about to check ability cooldown percent %d, valid=%d"), AbilitySlot, valid);
	float ret = 0.0f;
	if (!valid)
	{
		UE_LOG_FAST(TEXT("valid false return %f"), ret);
		return ret;
	}
	auto handle = AbilityHandles[AbilitySlot];
	auto spec = AbilitySystem->FindAbilitySpecFromHandle(handle);
	if (!spec)
	{
		return ret;
	}
	auto ability = spec->Ability;
	float remaining, duration;
	ability->GetCooldownTimeRemainingAndDuration(handle, ability->GetCurrentActorInfo(), remaining, duration);
	if (duration == 0.0f)
	{
		UE_LOG_FAST(TEXT("duration = 0, return %f"), ret);
		return ret;
	}
	ret = remaining / duration;
	UE_LOG_FAST(TEXT("return %f"), ret);
	return ret;
}

EAbilityState UAbilityUser::GetAbilityState(int AbilitySlot)
{
	if (!AbilityStates.IsValidIndex(AbilitySlot))
	{
		return EAbilityState::Invalid;
	}
	return AbilityStates[AbilitySlot];
}

bool UAbilityUser::GetAbilityAutoCastEnabled(int AbilitySlot)
{
	bool valid = AbilitySystem && AbilityHandles.IsValidIndex(AbilitySlot);
	if (!valid)
	{
		return false;
	}
	auto handle = AbilityHandles[AbilitySlot];
	auto spec = AbilitySystem->FindAbilitySpecFromHandle(handle);
	if (!spec)
	{
		return false;
	}
	bool implemented = spec->Ability->AbilityTags.HasAny(
		FGameplayTag::RequestGameplayTag("Combat.Autocast").GetSingleTagContainer());
	bool enabled = true;
	return implemented && enabled;
}

bool UAbilityUser::IsAbilityBusy(int AbilitySlot)
{
	bool valid = AbilitySystem && AbilityHandles.IsValidIndex(AbilitySlot);
	if (!valid)
	{
		return false;
	}
	auto handle = AbilityHandles[AbilitySlot];
	auto spec = AbilitySystem->FindAbilitySpecFromHandle(handle);
	if (!spec)
	{
		return false;
	}
	return spec->IsActive();
}

void UAbilityUser::RemoveAllAbilities()
{
	if (!GetOwner()->HasAuthority())
	{
		return;
	}
	AbilityHandles.Empty();
	AbilityStates.Empty();
	AbilitySystem->CancelAllAbilities();
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetOwner(),
		FGameplayTag::RequestGameplayTag("Event.TriggerPassiveDetach"), FGameplayEventData());
	const auto AllAbility = AbilitySystem->GetActivatableAbilities();
	for (auto Ability : AllAbility)
	{
		Ability.Level = 0;// not sure if this works
	}
	RecheckAbilitySlot();
	RecheckAbilityState();
	RecheckAbilityLevel();
}

int UAbilityUser::GetAbilityLevel(int AbilitySlot)
{
	bool valid = AbilitySystem && AbilityHandles.IsValidIndex(AbilitySlot);
	if (!valid)
	{
		return -1;
	}
	auto handle = AbilityHandles[AbilitySlot];
	auto spec = AbilitySystem->FindAbilitySpecFromHandle(handle);
	if (!spec)
	{
		return -1;
	}
	return spec->Level;
}

void UAbilityUser::ActivateAbility(int AbilitySlot)
{
	bool valid = AbilitySystem && AbilityHandles.IsValidIndex(AbilitySlot);
	UE_LOG_FAST(TEXT("about to activate ability %d, valid=%d"), AbilitySlot, valid);
	if (!valid)
	{
		return;
	}
	auto handle = AbilityHandles[AbilitySlot];
	auto spec = AbilitySystem->FindAbilitySpecFromHandle(handle);
	if (!spec)
	{
		return;
	}
	if (GetAbilityLevel(AbilitySlot) <= 0)
	{
		return;
	}
	{
		int n = UAbilitySystemGlobals::Get().TargetDataStructCache.ScriptStructs.Num();
		UE_LOG_FAST(TEXT("check targetdata script struct, got %d ready"), n);
	}
	AbilitySystem->TryActivateAbility(handle);
}

bool UAbilityUser::ActivateAbilityWithPayload(int AbilitySlot, FGameplayTag EventTag, FGameplayEventData Payload)
{
	bool valid = AbilitySystem && AbilityHandles.IsValidIndex(AbilitySlot);
	if (!valid)
	{
		return false;
	}
	auto handle = AbilityHandles[AbilitySlot];
	bool ret = AbilitySystem->TriggerAbilityFromGameplayEvent(
		handle, 
		AbilitySystem->AbilityActorInfo.Get(), 
		EventTag, &Payload, 
		*AbilitySystem);
	return ret;
}

void UAbilityUser::OnRep_AbilityStates()
{
	RecheckAbilityState();
}

void UAbilityUser::RecheckAbilityState()
{
	OnAbilityStateChanged.Broadcast();
}

void UAbilityUser::OnRep_AbilityBehaviors()
{

}

void UAbilityUser::OnRep_AbilityHandles()
{
	RecheckAbilitySlot();
}

void UAbilityUser::RecheckAbilitySlot()
{
	OnAbilitySlotChanged.Broadcast();
}

void UAbilityUser::RecheckAbilityLevel(int AbilitySlot)
{
	if (AbilitySlot != -1)
	{
		OnAbilityLevelChanged.Broadcast(AbilitySlot);
	}
	else
	{
		for (int i = 0; i< AbilityHandles.Num();i++)
		{
			OnAbilityLevelChanged.Broadcast(i);
		}
	}
}