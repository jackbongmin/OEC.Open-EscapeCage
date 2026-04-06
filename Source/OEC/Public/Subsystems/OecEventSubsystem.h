// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GameplayTagContainer.h"
#include "Data/OecEnumType.h"
#include "OecEventSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class OEC_API UOecEventSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	UFUNCTION(BlueprintCallable, Category = "OEC|Event")
	void ProcessQuestionResult(FGameplayTag InEventTag, float InPayloadValue, FName InPayloadString);

private:
	void HandleStatEffect(FGameplayTag InTag, float InValue);
	void HandleSpawnMonster(FName InMonsterID);
	void HandleGiveItem(FName InItemID);
};
