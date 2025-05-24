// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/ExecCalc/ExecCalc_Damage.h"

#include "AbilitySystemComponent.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/CharacterClassInfo.h"
#include "Interaction/CombatInterface.h"

struct AuraDamageStatic
{
	// 护甲
	DECLARE_ATTRIBUTE_CAPTUREDEF(Armor);
	// 护甲穿透
	DECLARE_ATTRIBUTE_CAPTUREDEF(ArmorPenetration);
	DECLARE_ATTRIBUTE_CAPTUREDEF(BlockChance);

	// Critical hit chance
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitChance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitDamage);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitResistance);

	DECLARE_ATTRIBUTE_CAPTUREDEF(FireResistance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(ArcaneResistance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(LightningResistance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(PhysicalResistance);

	TMap<FGameplayTag, FGameplayEffectAttributeCaptureDefinition> TagsToCaptureDefs;

	AuraDamageStatic()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, Armor, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, ArmorPenetration, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, BlockChance, Target, false);

		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CriticalHitChance, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CriticalHitDamage, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CriticalHitResistance, Target, false);


		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, FireResistance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, ArcaneResistance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, LightningResistance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, PhysicalResistance, Target, false);

		const FAuraGameplayTags& Tags = FAuraGameplayTags::Get();
		TagsToCaptureDefs.Add(Tags.Attributes_Secondary_Armor, ArmorDef);
		TagsToCaptureDefs.Add(Tags.Attributes_Secondary_ArmorPenetration, ArmorPenetrationDef);
		TagsToCaptureDefs.Add(Tags.Attributes_Secondary_BlockChance, BlockChanceDef);

		TagsToCaptureDefs.Add(Tags.Attributes_Secondary_CriticalHitChance, CriticalHitChanceDef);
		TagsToCaptureDefs.Add(Tags.Attributes_Secondary_CriticalHitDamage, CriticalHitDamageDef);
		TagsToCaptureDefs.Add(Tags.Attributes_Secondary_CriticalHitResistance, CriticalHitResistanceDef);

		TagsToCaptureDefs.Add(Tags.Attributes_Resistance_Arcane, ArcaneResistanceDef);
		TagsToCaptureDefs.Add(Tags.Attributes_Resistance_Lightning, LightningResistanceDef);
		TagsToCaptureDefs.Add(Tags.Attributes_Resistance_Fire, FireResistanceDef);
		TagsToCaptureDefs.Add(Tags.Attributes_Resistance_Physical, PhysicalResistanceDef);
	}
};

static const AuraDamageStatic& DamageStatic()
{
	static AuraDamageStatic DamageStatic;
	return DamageStatic;
}


UExecCalc_Damage::UExecCalc_Damage()
{
	RelevantAttributesToCapture.Add(DamageStatic().ArmorDef);
	RelevantAttributesToCapture.Add(DamageStatic().ArmorPenetrationDef);
	RelevantAttributesToCapture.Add(DamageStatic().BlockChanceDef);


	RelevantAttributesToCapture.Add(DamageStatic().CriticalHitChanceDef);
	RelevantAttributesToCapture.Add(DamageStatic().CriticalHitDamageDef);
	RelevantAttributesToCapture.Add(DamageStatic().CriticalHitResistanceDef);

	RelevantAttributesToCapture.Add(DamageStatic().FireResistanceDef);
	RelevantAttributesToCapture.Add(DamageStatic().ArcaneResistanceDef);
	RelevantAttributesToCapture.Add(DamageStatic().LightningResistanceDef);
	RelevantAttributesToCapture.Add(DamageStatic().PhysicalResistanceDef);
}

void UExecCalc_Damage::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
                                              FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	const UAbilitySystemComponent* SourceAbilitySystemComponent = ExecutionParams.GetSourceAbilitySystemComponent();
	const UAbilitySystemComponent* TargetAbilitySystemComponent = ExecutionParams.GetTargetAbilitySystemComponent();

	AActor* SourceAvatarActor = SourceAbilitySystemComponent
		                            ? SourceAbilitySystemComponent->GetAvatarActor()
		                            : nullptr;
	AActor* TargetAvatarActor = TargetAbilitySystemComponent
		                            ? TargetAbilitySystemComponent->GetAvatarActor()
		                            : nullptr;

	ICombatInterface* SourceCombatInterface = Cast<ICombatInterface>(SourceAvatarActor);
	ICombatInterface* TargetCombatInterface = Cast<ICombatInterface>(TargetAvatarActor);


	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();

	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluateParams;
	EvaluateParams.SourceTags = SourceTags;
	EvaluateParams.TargetTags = TargetTags;


	//
	// const FGameplayModifierEvaluatedData EvaluatedData(DamageStatic().ArmorProperty,EGameplayModOp::Additive,Armor);
	float DamageAmount = 0.f;

	for (const TTuple<FGameplayTag, FGameplayTag>& Pair : FAuraGameplayTags::Get().DamageTypesToResistances)
	{
		const FGameplayTag DamageTag = Pair.Key;
		const FGameplayTag ResistanceTag = Pair.Value;

		checkf(AuraDamageStatic().TagsToCaptureDefs.Contains(ResistanceTag),
		       TEXT("TagsToCaptureDefs doesn't contain Tag: [%s] in ExecCalc_Damage"), *ResistanceTag.ToString());

		const FGameplayEffectAttributeCaptureDefinition CaptureDef = AuraDamageStatic().TagsToCaptureDefs[
			ResistanceTag];

		float DamageTypeValue = Spec.GetSetByCallerMagnitude(Pair.Key);
		float Resistance = 0.f;
		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(CaptureDef, EvaluateParams, Resistance);
		Resistance = FMath::Clamp(Resistance, 0.f, 100.f);
		DamageTypeValue *= (100.f - Resistance) / 100.f;

		DamageAmount += DamageTypeValue;
	}

	float TargetBlockChance = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatic().BlockChanceDef, EvaluateParams,
	                                                           TargetBlockChance);
	TargetBlockChance = FMath::Max<float>(0.f, TargetBlockChance);

	// if Blocked, halve the damage 
	const bool bBlocked = FMath::RandRange(1, 100) < TargetBlockChance;

	FGameplayEffectContextHandle ContextHandle = Spec.GetContext();
	UAuraAbilitySystemLibrary::SetIsBlockedHit(ContextHandle, bBlocked);

	DamageAmount = bBlocked ? DamageAmount / 2.f : DamageAmount;


	float TargetArmor = 0;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatic().ArmorDef, EvaluateParams, TargetArmor);
	TargetArmor = FMath::Max<float>(0.f, TargetArmor);


	float SourceArmorPenetration = 0;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatic().ArmorPenetrationDef, EvaluateParams,
	                                                           SourceArmorPenetration);
	SourceArmorPenetration = FMath::Max<float>(0.f, SourceArmorPenetration);

	// get data from curve table
	const UCharacterClassInfo* CharacterClassInfo = UAuraAbilitySystemLibrary::GetCharacterClassInfo(SourceAvatarActor);
	const FRealCurve* ArmorPenetrationCurve = CharacterClassInfo->DamageCalculationCoefficients->FindCurve(
		FName("ArmorPenetration"), FString());
	const float ArmorPenetrationCoefficient = ArmorPenetrationCurve->Eval(SourceCombatInterface->GetCharacterLevel());


	// ArmorPenetration ignores a percentage of The Target's Armor
	const float EffectiveArmor = TargetArmor * (100.f - SourceArmorPenetration * ArmorPenetrationCoefficient) / 100.f;

	const FRealCurve* EffectiveArmorCurve = CharacterClassInfo->DamageCalculationCoefficients->FindCurve(
		FName("EffectiveArmor"), FString());
	const float EffectiveArmorCoefficient = EffectiveArmorCurve->Eval(TargetCombatInterface->GetCharacterLevel());

	// Armor ignores a percentage of Incoming Damage.e
	DamageAmount *= (100.f - EffectiveArmor * EffectiveArmorCoefficient) / 100.f;


	// Critical Hit Chance can plus damage
	float SourceCriticalChance = 0;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatic().CriticalHitChanceDef, EvaluateParams,
	                                                           SourceCriticalChance);
	SourceCriticalChance = FMath::Max<float>(0.f, SourceCriticalChance);

	float SourceCriticalDamage = 0;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatic().CriticalHitDamageDef, EvaluateParams,
	                                                           SourceCriticalDamage);
	SourceCriticalDamage = FMath::Max<float>(0.f, SourceCriticalDamage);


	float TargetCriticalResistance = 0;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatic().CriticalHitResistanceDef, EvaluateParams,
	                                                           TargetCriticalResistance);
	TargetCriticalResistance = FMath::Max<float>(0.f, TargetCriticalResistance);


	const FRealCurve* CriticalHitResistanceCurve = CharacterClassInfo->DamageCalculationCoefficients->FindCurve(
		FName("CriticalHitResistance"), FString());
	const float CriticalHitResistanceCoefficient = CriticalHitResistanceCurve->Eval(
		TargetCombatInterface->GetCharacterLevel());

	const float EffectiveCriticalHitChance = SourceCriticalChance - TargetCriticalResistance *
		CriticalHitResistanceCoefficient;
	const bool bCriticalHit = FMath::RandRange(0, 100) < EffectiveCriticalHitChance;

	UAuraAbilitySystemLibrary::SetIsCriticalHit(ContextHandle, bCriticalHit);

	DamageAmount = bCriticalHit ? DamageAmount * (2 + SourceCriticalDamage / 100.f) : DamageAmount;


	const FGameplayModifierEvaluatedData EvaluatedData(UAuraAttributeSet::GetIncomingDamageAttribute(),
	                                                   EGameplayModOp::Additive, DamageAmount);
	OutExecutionOutput.AddOutputModifier(EvaluatedData);
}
