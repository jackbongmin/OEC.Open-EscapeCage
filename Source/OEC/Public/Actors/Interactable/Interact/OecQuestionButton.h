// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/Interactable/OecInteractableBase.h"
#include "GameplayTagContainer.h"
#include "OecQuestionButton.generated.h"

/**
 * 
 */
UCLASS()
class OEC_API AOecQuestionButton : public AOecInteractableBase
{
	GENERATED_BODY()
	
public:
    virtual void Interact_Implementation(AActor* InInteractor) override;

    UFUNCTION(BlueprintCallable, Category = "OEC|Event")
    void SetButtonEventData(FGameplayTag InTag, float InPayloadValue, FName InPayloadString);

protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "OEC|Event Data")
    FGameplayTag EventTag;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "OEC|Event Data")
    float PayloadValue;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "OEC|Event Data")
    FName PayloadString;

private:
    void PlayButtonPressFeedback();
    void TriggerButtonEvent();

};
