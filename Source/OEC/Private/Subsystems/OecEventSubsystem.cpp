// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystems/OecEventSubsystem.h"
#include "Subsystems/OecStageManagerSubsystem.h"
#include "NativeGameplayTags.h"

void UOecEventSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void UOecEventSubsystem::ProcessQuestionResult(FGameplayTag InEventTag, float InPayloadValue, FName InPayloadString)
{
    if (InEventTag.MatchesTag(FGameplayTag::RequestGameplayTag(TEXT("Event.Stack.Add"))))
    {
        if (UOecStageManagerSubsystem* stageManager = GetGameInstance()->GetSubsystem<UOecStageManagerSubsystem>())
        {
            stageManager->AddStack(static_cast<int32>(InPayloadValue));
        }
    }

    if (InEventTag.MatchesTag(FGameplayTag::RequestGameplayTag(TEXT("Event.Spawn.Monster"))))
    {
        HandleSpawnMonster(InPayloadString);
    }

    UE_LOG(LogTemp, Log, TEXT("버튼 처리 완료. 태그: %s"), *InEventTag.ToString());
}

void UOecEventSubsystem::HandleStatEffect(FGameplayTag InTag, float InValue)
{
	//TODO : 태그에 따라 스탯 효과 적용 로직 구현
    UE_LOG(LogTemp, Log, TEXT("스탯 효과 적용: %s, 값: %f"), *InTag.ToString(), InValue);
}

void UOecEventSubsystem::HandleSpawnMonster(FName InMonsterID)
{
	//TODO : 몬스터 ID에 따라 몬스터 스폰 로직 구현
    UE_LOG(LogTemp, Log, TEXT("몬스터 스폰 명령: %s"), *InMonsterID.ToString());
}

void UOecEventSubsystem::HandleGiveItem(FName InItemID)
{
	//TODO : 아이템 ID에 따라 아이템 지급 로직 구현
    UE_LOG(LogTemp, Log, TEXT("아이템 지급 명령: %s"), *InItemID.ToString());
}
