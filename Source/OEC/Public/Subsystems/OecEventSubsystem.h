// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GameplayTagContainer.h"
#include "Data/OecEnumType.h"
#include "Data/OecDataStruct.h"
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
	void ProcessQuestionResult(FName InQuestionID, FGameplayTag InTag, float InValue, FName InString);

	bool IsQuestionUsed(FName InQuestionID) const;

	void ResetUsedQuestions();

	UFUNCTION(BlueprintCallable, Category = "OEC|Stack")
	int32 GetCurrentStacks() const { return PlayerStacks; }

	UFUNCTION(BlueprintCallable, Category = "OEC|Stack")
	void ModifyStacks(int32 InAmount);

private:
	UPROPERTY()
	TArray<FChoiceRecord> ChoiceHistory;

	UPROPERTY()
	int32 PlayerStacks = 0;;

	void RecordChoice(FGameplayTag InTag, float InValue);

private:
	UPROPERTY()
	TArray<FName> UsedQuestionIDs;

private:
	void HandleStatEffect(FGameplayTag InTag, float InValue);
	void HandleSpawnMonster(FName InMonsterID);
	void HandleGiveItem(FName InItemID);
};
