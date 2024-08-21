// Copyright zhangnengwei

#pragma once

#include "CoreMinimal.h"
#include "Interaction/EnemyInterface.h"
#include "GameFramework/PlayerController.h"
#include "AuraPlayerController.generated.h"

class UInputMappingContext;
class UInputAction;

struct FInputActionValue;
class IEnemyInterface;
/**
 * 
 */
UCLASS()
class AURA_API AAuraPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AAuraPlayerController();

	// 高亮显示敌人 需要每帧执行检查
	virtual void PlayerTick(float DeltaTime) override;
protected:
	virtual void BeginPlay() override;
	
	virtual void SetupInputComponent() override;

private:

	// 将映射上下文公开为头文件中的属性...
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputMappingContext> AuraContext;


	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> MoveAction;
	
	void Move(const FInputActionValue& InputActionVaule);

	// 自定义鼠标跟踪方法
	void CursorTrace();

	IEnemyInterface* LastActor;
	IEnemyInterface* ThisActor;
};
