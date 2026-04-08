// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Interactable/Interact/OecQuestionButton.h"
#include "Subsystems/OecEventSubsystem.h"

void AOecQuestionButton::Interact_Implementation(AActor* InInteractor)
{
    Super::Interact_Implementation(InInteractor);

    PlayButtonPressFeedback();
    TriggerButtonEvent();

    if (OnButtonClicked.IsBound())
    {
        OnButtonClicked.Broadcast();
    }
}

void AOecQuestionButton::SetButtonEventData(FName InQuestionID, FGameplayTag InTag, float InPayloadValue, FName InPayloadString)
{
    QuestionID = InQuestionID;
    EventTag = InTag;
    PayloadValue = InPayloadValue;
    PayloadString = InPayloadString;

    UE_LOG(LogTemp, Log, TEXT("버튼 데이터 갱신 완료: %s"), *EventTag.ToString());
}

void AOecQuestionButton::PlayButtonPressFeedback()
{
    UE_LOG(LogTemp, Log, TEXT("버튼 눌림"));
}

void AOecQuestionButton::TriggerButtonEvent()
{
    if (!bIsTerminalButton)
    {
        UE_LOG(LogTemp, Log, TEXT("일반 상호작용 버튼입니다. 스택 로직을 무시합니다."));
        return;
    }

    if (UOecEventSubsystem* eventSubsystem = GetGameInstance()->GetSubsystem<UOecEventSubsystem>())
    {
        eventSubsystem->ProcessQuestionResult(QuestionID, EventTag, PayloadValue, PayloadString);
    }
}
