// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AuraAbilitySystemComponent.generated.h"

// 广播Tag
DECLARE_MULTICAST_DELEGATE_OneParam(FEffectAssetsTags, const FGameplayTagContainer& /*AssetsTag*/);

/**
 * 
 */
UCLASS()
class AURA_API UAuraAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	void SetAbilityActorInfo();

	FEffectAssetsTags OnAbilityAssetTagChanged;

	void AddCharacterAbilities(TArray<TSubclassOf<UGameplayAbility>>& StartupAbilities);

	void AbilityInputTagHeld(const FGameplayTag& InputTag);
	void AbilityInputTagReleased(const FGameplayTag& InputTag);

protected:

	UFUNCTION(Client, Reliable)
	void ClientEffectApplied(UAbilitySystemComponent* AbilitySystemComponent,
	                   const FGameplayEffectSpec& GameplayEffectSpec,
	                   FActiveGameplayEffectHandle ActiveGameplayEffectHandle);
};
