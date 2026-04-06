// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Terminal/OecTerminalBase.h"
#include "Actors/Interactable/Interact/OecQuestionButton.h"
#include "Components/StaticMeshComponent.h"
#include "Components/WidgetComponent.h"
#include "Data/OecDataStruct.h"

// Sets default values
AOecTerminalBase::AOecTerminalBase()
{
	PrimaryActorTick.bCanEverTick = true;

}

void AOecTerminalBase::InitializeTerminal()
{
    if (!QuestionDataTable)
    {
        UE_LOG(LogTemp, Error, TEXT("단말기에 질문 데이터 테이블이 연결되지 않았음"));
        return;
    }

    // 타입별로 질문을 받을 준비
    TMap<EQuestionType, TArray<FQuestionStaticData*>> categorizedQuestions;

    // 데이터 테이블의 모든 행 데이터를 가져오기
    TArray<FQuestionStaticData*> allRows;
    QuestionDataTable->GetAllRows(TEXT("TerminalInit"), allRows);

    if (allRows.Num() == 0) return;

    // 바구니에 타입별로 분류해서 담기
    for (FQuestionStaticData* row : allRows)
    {
        if (row)
        {
            categorizedQuestions.FindOrAdd(row->QuestionType).Add(row);
        }
    }

    // 전체 확률 총합 구하기
    float totalWeight = 0.0f;
    for (const auto& pair : categorizedQuestions)
    {
        if (QuestionTypeWeights.Contains(pair.Key) && pair.Value.Num() > 0)
        {
            totalWeight += QuestionTypeWeights[pair.Key];
        }
    }

    if (totalWeight <= 0.0f)
    {
        UE_LOG(LogTemp, Error, TEXT("단말기 확률 세팅이 안 되어 있거나 데이터가 없습니다!"));
        return;
    }

    // 주사위 굴리기
    float randomRoll = FMath::FRandRange(0.0f, totalWeight);
    EQuestionType selectedType = EQuestionType::Balance;
    float accumulatedWeight = 0.0f;

    // 어떤 타입에 당첨되었는지 검사
    for (const auto& pair : categorizedQuestions)
    {
        if (QuestionTypeWeights.Contains(pair.Key) && pair.Value.Num() > 0)
        {
            accumulatedWeight += QuestionTypeWeights[pair.Key];
            if (randomRoll <= accumulatedWeight)
            {
                selectedType = pair.Key;
                break;
            }
        }
    }

    // 당첨된 타입의 바구니 안에서 진짜 질문 1개 랜덤으로 뽑기
    TArray<FQuestionStaticData*>& availableQuestions = categorizedQuestions[selectedType];
    int32 randomIndex = FMath::RandRange(0, availableQuestions.Num() - 1);
    FQuestionStaticData* finalQuestion = availableQuestions[randomIndex];

    // 단말기 로그 출력 및 버튼에 데이터 덮어씌우기
    if (finalQuestion)
    {
        UE_LOG(LogTemp, Log, TEXT("===================================="));
        UE_LOG(LogTemp, Log, TEXT("뽑힌 타입: %d | 단말기 화면: %s"), (int32)selectedType, *finalQuestion->QuestionText);
        UE_LOG(LogTemp, Log, TEXT("===================================="));

        UpdateTerminalUI(finalQuestion->QuestionText);

        if (RedButton)
        {
            RedButton->SetButtonEventData(finalQuestion->RedBtnEventTag, finalQuestion->RedBtnPayloadValue, finalQuestion->RedBtnPayloadString);
        }

        if (BlueButton)
        {
            BlueButton->SetButtonEventData(finalQuestion->BlueBtnEventTag, finalQuestion->BlueBtnPayloadValue, finalQuestion->BlueBtnPayloadString);
        }
    }
}

void AOecTerminalBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AOecTerminalBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

