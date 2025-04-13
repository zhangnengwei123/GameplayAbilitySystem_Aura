// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AuraUserWidget.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UAuraUserWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	/**蓝图访问这个方法，会执行下面的动作
	 * 设置WidgetController,
	 * 触发WidgetControllerSet事件，这个事件也可以在蓝图实现。
	 *
	 * 这样就串联起来了
	 */
	UFUNCTION(BlueprintCallable)
	void SetWidgetController(UObject* InWidgetController);

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UObject> WidgetController;

protected:
	
	UFUNCTION(BlueprintImplementableEvent)
	void WidgetControllerSet();
};
