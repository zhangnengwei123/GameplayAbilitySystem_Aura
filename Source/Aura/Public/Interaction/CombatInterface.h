// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CombatInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI,BlueprintType)
class UCombatInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class AURA_API ICombatInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	/*
	 * 建立接口从角色中获取等级 敌人和玩家都实现这个接口
	 */
	virtual int32 GetCharacterLevel();

	virtual FVector GetCombatSocketLocation();

	UFUNCTION(BlueprintImplementableEvent,BlueprintCallable)
	void UpdateFacingTarget(const FVector& NewFacingTarget);

	UFUNCTION(BlueprintNativeEvent,BlueprintCallable)
	UAnimMontage* GetHitReactMontage();

	virtual void Die() = 0;
};
