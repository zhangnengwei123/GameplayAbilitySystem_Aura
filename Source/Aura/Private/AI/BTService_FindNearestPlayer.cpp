// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTService_FindNearestPlayer.h"

#include "AIController.h"
#include "BehaviorTree/BTFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"

void UBTService_FindNearestPlayer::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	// if (GEngine)
	// GEngine->AddOnScreenDebugMessage(1, 15.0f, FColor::Red, *AIOwner->GetName());
	// GEngine->AddOnScreenDebugMessage(2, 15.0f, FColor::Green, *ActorOwner->GetName());

	/* Breakdown of the inputs:
	-1 = A unique key to prevent the same message from being added multiple times. Set it to -1 if the uniqueness doesn't matter to you
	15.0f = How long to display the message, in seconds.
	FColor::Red = The color in which it should get printed to the screen
	*/

	APawn* OwningPawn = AIOwner->GetPawn();
	const FName TargetTag = OwningPawn->ActorHasTag(FName("Player")) ? FName("Enemy") : FName("Player");

	TArray<AActor*> FoundNearestPlayers;
	UGameplayStatics::GetAllActorsWithTag(OwningPawn, TargetTag, FoundNearestPlayers);

	float ClosestDistance = TNumericLimits<float>::Max();
	AActor* ClosestActor = nullptr;
	for (AActor* FoundNearestPlayer : FoundNearestPlayers)
	{
		// if (GEngine)
		// 	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, *FoundNearestPlayer->GetName());
		//
		if (IsValid(FoundNearestPlayer) && IsValid(OwningPawn))
		{
			const float Distance = OwningPawn->GetDistanceTo(FoundNearestPlayer);
			if (Distance < ClosestDistance)
			{
				ClosestActor = FoundNearestPlayer;
				ClosestDistance = Distance;
			}
		}
	}

	UBTFunctionLibrary::SetBlackboardValueAsObject(this,TargetToFollowSelector,ClosestActor);
	UBTFunctionLibrary::SetBlackboardValueAsFloat(this,DistanceToTargetSelector,ClosestDistance);
	
}
