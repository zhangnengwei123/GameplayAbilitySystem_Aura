// Copyright zhangnengwei

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AuraEffectActor.generated.h"

class UGameplayEffect;

UCLASS()
class AURA_API AAuraEffectActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAuraEffectActor();


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	void ApplyEffectToTarget(AActor* TargetActor, TSubclassOf<UGameplayEffect> GameplayEffectClass);


	// 一次性的游戏效果
	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category = "Applied EffectS")
	TSubclassOf<UGameplayEffect> InstantGameplayEffectClass;

	// 持续的游戏效果
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Applied EffectS")
	TSubclassOf<UGameplayEffect> DurationGameplayEffectClass;

};
