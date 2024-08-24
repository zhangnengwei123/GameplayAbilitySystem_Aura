// Copyright zhangnengwei


#include "Actor/AuraEffectActor.h"
#include "AbilitySystemBlueprintLibrary.h"

#include "AbilitySystemComponent.h"


// Sets default values
AAuraEffectActor::AAuraEffectActor()
{
 
	PrimaryActorTick.bCanEverTick = false;
	SetRootComponent(CreateDefaultSubobject<USceneComponent>("SceneRoot"));
	
}



void AAuraEffectActor::BeginPlay()
{
	Super::BeginPlay();
	
	
}

void AAuraEffectActor::ApplyEffectToTarget(AActor* TargetActor, TSubclassOf<UGameplayEffect> GameplayEffectClass)
{
	//// 实现了接口的情况可以使用这种方式获取能力组件
	//IAbilitySystemInterface* ASCInterface = Cast<IAbilitySystemInterface>(TargetActor);
	//if (ASCInterface)
	//{
	//	ASCInterface->GetAbilitySystemComponent();
	//}

	// 还有一种使用能力系统函数的方式
	UAbilitySystemComponent* TargetAbilitySystemComponent =  UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
	if (TargetAbilitySystemComponent == nullptr) return;
	check(GameplayEffectClass);
	// 应用效果
	FGameplayEffectContextHandle EffectContextHandle = TargetAbilitySystemComponent->MakeEffectContext();
	EffectContextHandle.AddSourceObject(this);

	const FGameplayEffectSpecHandle EffectSpecHandle = 
		TargetAbilitySystemComponent->MakeOutgoingSpec(GameplayEffectClass, ActorLevel, EffectContextHandle);
	
	const FActiveGameplayEffectHandle ActiveGameplayEffectHandle = 
		TargetAbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data.Get());

	// 从GameplayEffectSpecHandle中获取GameplayEffect,然后再获取GameplayEffect的期间策略,判断是否是永久性的。
	const bool bIsInfinite =  EffectSpecHandle.Data.Get()->Def.Get()->DurationPolicy == EGameplayEffectDurationType::Infinite;

	// 满足条件： 永久效果 并且 需要在结束重叠时移除效果 
	// 才需要添加到Map中
	if (bIsInfinite && InfiniteEffectRemovalPolicy == EEffectRemovalPolicy::RemoveOnEndOverlap)
	{
		ActiveEffectHandles.Add(ActiveGameplayEffectHandle, TargetAbilitySystemComponent);
	}
}

void AAuraEffectActor::OnBegingOverlap(AActor* TargetActor)
{
	if (InstantEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnBeginOverlap)
	{
		ApplyEffectToTarget(TargetActor, InstantGameplayEffectClass);
	}

	if (DurationEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnBeginOverlap)
	{
		ApplyEffectToTarget(TargetActor, DurationGameplayEffectClass);
	}

	if (InfiniteEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnBeginOverlap)
	{
		ApplyEffectToTarget(TargetActor, InfiniteGameplayEffectClass);
	}
}

void AAuraEffectActor::OnEndOverlap(AActor* TargetActor)
{
	if (InstantEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnEndOverlap)
	{
		ApplyEffectToTarget(TargetActor, InstantGameplayEffectClass);
	}

	if (DurationEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnEndOverlap)
	{
		ApplyEffectToTarget(TargetActor, DurationGameplayEffectClass);
	}

	if (InfiniteEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnEndOverlap)
	{
		ApplyEffectToTarget(TargetActor, InfiniteGameplayEffectClass);
	}

	// 移除效果
	if (InfiniteEffectRemovalPolicy == EEffectRemovalPolicy::RemoveOnEndOverlap)
	{
		UAbilitySystemComponent* TargetAbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
		// 判断能力组件是否有效 无效直接返回
		if (!IsValid(TargetAbilitySystemComponent)) return;

		TArray<FActiveGameplayEffectHandle> HandlesToRemove;
		// 遍历Map
		for (TTuple<FActiveGameplayEffectHandle, UAbilitySystemComponent*> HandlePair : ActiveEffectHandles)
		{
			// 如果找到了 那么就移除效果 并删除Map中内容
			// 但是不能在这里删除 因为在for循环遍历中操作删除会导致崩溃 使用一个变量接收删除掉的数据 在后面进行处理
			if (TargetAbilitySystemComponent == HandlePair.Value)
			{
				TargetAbilitySystemComponent->RemoveActiveGameplayEffect(HandlePair.Key,1);
				HandlesToRemove.Add(HandlePair.Key);
			}

		}

		for (FActiveGameplayEffectHandle& Handle : HandlesToRemove)
		{
			ActiveEffectHandles.FindAndRemoveChecked(Handle);
		}


	}
}


