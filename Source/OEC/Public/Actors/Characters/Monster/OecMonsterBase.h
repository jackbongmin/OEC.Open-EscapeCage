// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/Characters/OecCharacterBase.h"
#include "OecMonsterBase.generated.h"

/**
 * 
 */
class UOecAttributeSetBase;

UCLASS()
class OEC_API AOecMonsterBase : public AOecCharacterBase
{
	GENERATED_BODY()
	
public:
	AOecMonsterBase();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "OEC|GAS", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UOecAttributeSetBase> AttributeSet;

};
