// Fill out your copyright notice in the Description page of Project Settings.


#include "Input/AuraInputConfig.h"


const UInputAction* UAuraInputConfig::FindAbilityInputActionForTag(const FGameplayTag& GameplayTag, bool bLogNotFound) const
{
	for (const FAuraInputAction& AuraInputAction : AbilityInputActions)
	{
		if (AuraInputAction.InputAction && AuraInputAction.GameplayTag == GameplayTag)
		{
			return AuraInputAction.InputAction;
		}
	}

	if (bLogNotFound)
	{
		UE_LOG(LogTemp, Error, TEXT("Can not find AbilityInputAction for tag: %s"), *GameplayTag.ToString());
	}
	return nullptr;
}
