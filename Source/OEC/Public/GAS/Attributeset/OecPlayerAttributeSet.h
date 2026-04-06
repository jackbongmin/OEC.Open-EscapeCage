// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/Attributeset/OecAttributeSetBase.h"
#include "OecPlayerAttributeSet.generated.h"

/**
 * 
 */
UCLASS()
class OEC_API UOecPlayerAttributeSet : public UOecAttributeSetBase
{
	GENERATED_BODY()
	
public:
    UPROPERTY(BlueprintReadOnly, Category = "OEC|Attributes")
    FGameplayAttributeData Sanity;
    ATTRIBUTE_ACCESSORS(UOecPlayerAttributeSet, Sanity)

    UPROPERTY(BlueprintReadOnly, Category = "Attributes")
    FGameplayAttributeData AnswerCount;
    ATTRIBUTE_ACCESSORS(UOecPlayerAttributeSet, AnswerCount)
};
