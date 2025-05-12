// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/AuraPlayerController.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AuraGameplayTags.h"
#include "EnhancedInputSubsystems.h"
#include "MovieSceneTracksComponentTypes.h"
#include "NavigationPath.h"
#include "NavigationSystem.h"
#include "Input/AuraInputComponent.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "Components/SplineComponent.h"
#include "GameFramework/Character.h"
#include "Interaction/EnemyInterface.h"
#include "UI/Widget/DamageTextComponent.h"

AAuraPlayerController::AAuraPlayerController()
{
	bReplicates = true;

	Spline = CreateDefaultSubobject<USplineComponent>("Spline");
}

void AAuraPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);
	CursorTrace();
	AutoRun();
}

void AAuraPlayerController::ShowDamageNumber_Implementation(float DamageAmount, ACharacter* TargetCharacter)
{
	if (IsValid(TargetCharacter) && DamageTextComponentClass)
	{
		UDamageTextComponent* DamageTextComponent = NewObject<UDamageTextComponent>(TargetCharacter, DamageTextComponentClass);
		DamageTextComponent->RegisterComponent();
		DamageTextComponent->AttachToComponent(TargetCharacter->GetRootComponent(),
		                                       FAttachmentTransformRules::KeepRelativeTransform);
		DamageTextComponent->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
		DamageTextComponent->SetDamageText(DamageAmount);
	}
}

void AAuraPlayerController::AutoRun()
{
	if (!bAutoRunning) return;
	if (APawn* ControlledPawn = GetPawn())
	{
		const FVector LocationOnSpline = Spline->FindLocationClosestToWorldLocation(
			ControlledPawn->GetActorLocation(), ESplineCoordinateSpace::World);
		const FVector Direction = Spline->FindDirectionClosestToWorldLocation(
			LocationOnSpline, ESplineCoordinateSpace::World);
		ControlledPawn->AddMovementInput(Direction);

		const float DistanceToDestination = (LocationOnSpline - CachedDestination).Length();
		if (DistanceToDestination <= AutoRunAcceptanceRadius)
		{
			bAutoRunning = false;
		}
	}
}

void AAuraPlayerController::BeginPlay()
{
	Super::BeginPlay();
	check(AuraContext);

	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(
		GetLocalPlayer());
	if (Subsystem != nullptr)
	{
		Subsystem->AddMappingContext(AuraContext, 0);
	}
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;
	FInputModeGameAndUI InputMode;
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputMode.SetHideCursorDuringCapture(false);
	SetInputMode(InputMode);
}

void AAuraPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	UAuraInputComponent* AuraInputComponent = CastChecked<UAuraInputComponent>(InputComponent);
	AuraInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AAuraPlayerController::Move);
	AuraInputComponent->BindAction(ShiftAction, ETriggerEvent::Started, this, &AAuraPlayerController::ShiftPressed);
	AuraInputComponent->BindAction(ShiftAction, ETriggerEvent::Completed, this, &AAuraPlayerController::ShiftReleased);


	AuraInputComponent->BindAbilityInputActions(InputConfig, this,
	                                            &ThisClass::AbilityInputTagPressed,
	                                            &ThisClass::AbilityInputTagReleased,
	                                            &ThisClass::AbilityInputTagHeld);
}

void AAuraPlayerController::Move(const FInputActionValue& InputActionValue)
{
	const FVector2D InputAxisVector = InputActionValue.Get<FVector2D>();
	const FRotator Rotation = GetControlRotation();
	const FRotator YarRotation(0.f, Rotation.Yaw, 0.f);

	const FVector ForwardDirection = FRotationMatrix(YarRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YarRotation).GetUnitAxis(EAxis::Y);

	if (APawn* ControlledPawn = GetPawn())
	{
		ControlledPawn->AddMovementInput(ForwardDirection, InputAxisVector.Y);
		ControlledPawn->AddMovementInput(RightDirection, InputAxisVector.X);
	}
}

void AAuraPlayerController::CursorTrace()
{
	GetHitResultUnderCursor(ECC_Visibility, false, CursorHit);
	if (!CursorHit.bBlockingHit) return;

	LastActor = ThisActor;
	ThisActor = Cast<IEnemyInterface>(CursorHit.GetActor());

	/**
	* Line trace from cursor.There are serval scenarios:
	*  A. LastActor is null && ThisActor is null
	*		- Do nothing
	*  B. LastActor is null && ThisActor is valid
	*		- Highlight ThisActor
	*  C. LastActor is valid && ThisActor is null
	*		- UnHighlight LastActor
	*  D. Both actors are valid, but LastActor != ThisActor
	*		- UnHighlight LastActor,And Highlight ThisActor
	*  E. Both actors are valid, and LastActor == ThisActor
	*		- Do nothing
	* 
	* 
	*/
	// if (LastActor == nullptr)
	// {
	// 	if (ThisActor != nullptr)
	// 	{
	// 		// case B
	// 		ThisActor->HighlightActor();
	// 	}
	// 	else
	// 	{
	// 		// case A
	// 	}
	// }
	// else
	// {
	// 	if (ThisActor == nullptr)
	// 	{
	// 		// case C
	// 		LastActor->UnHighlightActor();
	// 	}
	// 	else
	// 	{
	// 		if (LastActor != ThisActor)
	// 		{
	// 			// case D
	// 			LastActor->UnHighlightActor();
	// 			ThisActor->HighlightActor();
	// 		}
	// 		else
	// 		{
	// 			// case E
	// 		}
	// 	}
	// }
	// Let's implement the logic simply.

	if (LastActor != ThisActor)
	{
		if (LastActor) LastActor->UnHighlightActor();
		if (ThisActor) ThisActor->HighlightActor();
	}
}

void AAuraPlayerController::AbilityInputTagPressed(FGameplayTag GameplayTag)
{
	// GEngine->AddOnScreenDebugMessage(1, 3.0f, FColor::Red, *GameplayTag.ToString());
	if (GameplayTag.MatchesTagExact(FAuraGameplayTags::Get().Input_LMB))
	{
		bTargeting = ThisActor ? true : false;
		bAutoRunning = false;
	}
}

void AAuraPlayerController::AbilityInputTagReleased(FGameplayTag GameplayTag)
{
	// GEngine->AddOnScreenDebugMessage(2, 3.0f, FColor::Blue, *GameplayTag.ToString());
	// if (GetAuraAbilitySystemComponent() == nullptr) return;
	// GetAuraAbilitySystemComponent()->AbilityInputTagReleased(GameplayTag);

	if (!GameplayTag.MatchesTagExact(FAuraGameplayTags::Get().Input_LMB))
	{
		if (GetAuraAbilitySystemComponent())
		{
			GetAuraAbilitySystemComponent()->AbilityInputTagReleased(GameplayTag);
		}
		return;
	}

	if (GetAuraAbilitySystemComponent())
	{
		GetAuraAbilitySystemComponent()->AbilityInputTagReleased(GameplayTag);
	}

	if (!bTargeting && !bShiftKeyDown)
	{
		// move
		const APawn* ControlledPawn = GetPawn();
		if (FollowTime <= ShortPressedThreshold && ControlledPawn)
		{
			if (UNavigationPath* NavigationPath =
				UNavigationSystemV1::FindPathToLocationSynchronously(this,
				                                                     ControlledPawn->GetActorLocation(),
				                                                     CachedDestination))
			{
				Spline->ClearSplinePoints();
				for (const FVector& PointLoc : NavigationPath->PathPoints)
				{
					Spline->AddSplinePoint(PointLoc, ESplineCoordinateSpace::World);
					// DrawDebugSphere(GetWorld(), PointLoc, 8.0f, 8, FColor::Green, false, 5.0f);
				}
				if (NavigationPath->PathPoints.Num() > 0)
				{
					CachedDestination = NavigationPath->PathPoints[NavigationPath->PathPoints.Num() - 1];
				}
				bAutoRunning = true;
			}
		}
		FollowTime = 0.0f;
		bTargeting = false;
	}
}

void AAuraPlayerController::AbilityInputTagHeld(FGameplayTag GameplayTag)
{
	// GEngine->AddOnScreenDebugMessage(3, 3.0f, FColor::Green, *GameplayTag.ToString());

	if (!GameplayTag.MatchesTagExact(FAuraGameplayTags::Get().Input_LMB))
	{
		if (GetAuraAbilitySystemComponent())
		{
			GetAuraAbilitySystemComponent()->AbilityInputTagHeld(GameplayTag);
		}
		return;
	}

	if (bTargeting || bShiftKeyDown)
	{
		if (GetAuraAbilitySystemComponent())
		{
			GetAuraAbilitySystemComponent()->AbilityInputTagHeld(GameplayTag);
		}
	}
	else
	{
		// move
		FollowTime += GetWorld()->GetDeltaSeconds();
		// We use hit result from Cursor Trace Function,do not need to get here again.

		// FHitResult Hit;
		// if (GetHitResultUnderCursor(ECC_Visibility, false, Hit))
		// {
		// 	CachedDestination = Hit.ImpactPoint;
		// }
		if (CursorHit.bBlockingHit)
		{
			CachedDestination = CursorHit.ImpactPoint;
		}

		if (APawn* ControlledPawn = GetPawn())
		{
			const FVector WorldDirection = (CachedDestination - ControlledPawn->GetActorLocation()).GetSafeNormal();
			ControlledPawn->AddMovementInput(WorldDirection);
		}
	}
}

UAuraAbilitySystemComponent* AAuraPlayerController::GetAuraAbilitySystemComponent()
{
	if (AuraAbilitySystemComponent == nullptr)
	{
		AuraAbilitySystemComponent = Cast<UAuraAbilitySystemComponent>(
			UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPawn()));
	}
	return AuraAbilitySystemComponent;
}
