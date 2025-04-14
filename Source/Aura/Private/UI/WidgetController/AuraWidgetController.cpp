// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/WidgetController/AuraWidgetController.h"

void UAuraWidgetController::SetWidgetControllerParams(const FWidgetControllerParams& widgetControllerParams)
{
	PlayerController = widgetControllerParams.PlayerController;
	PlayerState = widgetControllerParams.PlayerState;
	AbilitySystemComponent = widgetControllerParams.AbilitySystemComponent;
	AttributeSet = widgetControllerParams.AttributeSet;
}

void UAuraWidgetController::BroadcastInitialValue()
{
}

void UAuraWidgetController::BindCallbacksToDependencies()
{
}
