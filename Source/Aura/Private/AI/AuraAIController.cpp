// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/AuraAIController.h"

#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"

AAuraAIController::AAuraAIController()
{
	Blackboard  = CreateDefaultSubobject<UBlackboardComponent>("Blackboard Component");
	check(Blackboard);
	BehaviorTreeComponent = CreateDefaultSubobject<UBehaviorTreeComponent>("Behavior Tree Component");
	check(BehaviorTreeComponent);
}
