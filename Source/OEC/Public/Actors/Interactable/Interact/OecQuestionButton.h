// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/Interactable/OecInteractableBase.h"
#include "GameplayTagContainer.h"
#include "OecQuestionButton.generated.h"

/**
 * 
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnButtonClickedDelegate);

UCLASS()
class OEC_API AOecQuestionButton : public AOecInteractableBase
{
	GENERATED_BODY()
	
public:
    virtual void Interact_Implementation(AActor* InInteractor) override;

    UFUNCTION(BlueprintCallable, Category = "OEC|Event")
    void SetButtonEventData(FName InQuestionID, FGameplayTag InTag, float InPayloadValue, FName InPayloadString);

protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "OEC|Event Data")
    FGameplayTag EventTag;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "OEC|Event Data")
    float PayloadValue;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "OEC|Event Data")
    FName PayloadString;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "OEC|Event Data")
    FName QuestionID;

    UPROPERTY(EditAnywhere, Category = "OEC|Event")
    bool bIsTerminalButton = true;

public:
    UPROPERTY(BlueprintAssignable, Category = "OEC|Event")
    FOnButtonClickedDelegate OnButtonClicked;

private:
    void PlayButtonPressFeedback();
    void TriggerButtonEvent();

};
