// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/AuraAbilitySystemComponent.h"

void UAuraAbilitySystemComponent::SetAbilityActorInfo()
{
	OnGameplayEffectAppliedDelegateToSelf.AddUObject(this, &UAuraAbilitySystemComponent::EffectApplied);
}

void UAuraAbilitySystemComponent::EffectApplied(UAbilitySystemComponent* AbilitySystemComponent,
                                                const FGameplayEffectSpec& GameplayEffectSpec,
                                                FActiveGameplayEffectHandle ActiveGameplayEffectHandle)
{
	// GEngine->AddOnScreenDebugMessage(1, 5, FColor::Red, "EffectApplied for AbilitySystemComponent");

	FGameplayTagContainer GameplayTags;
	GameplayEffectSpec.GetAllAssetTags(GameplayTags);
	
	// 广播Tags
	OnAbilityAssetTagChanged.Broadcast(GameplayTags);
	
	
}
