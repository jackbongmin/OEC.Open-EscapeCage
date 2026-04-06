// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class EQuestionType : uint8
{
    Story       UMETA(DisplayName = "스토리"),
    Bonus       UMETA(DisplayName = "보너스"),
    Penalty     UMETA(DisplayName = "패널티"),
    Balance     UMETA(DisplayName = "밸런스"),
    Meaningless UMETA(DisplayName = "무의미")
};

UENUM(BlueprintType)
enum class ERoomType : uint8
{
    Safe,
    Supply,
    Danger,
    Exit
};

UENUM(BlueprintType)
enum class EItemType : uint8
{
    Weapon      UMETA(DisplayName = "무기"),
    Consumable  UMETA(DisplayName = "소모품"),
    KeyItem     UMETA(DisplayName = "퀘스트아이템")
};

UENUM(BlueprintType)
enum class ESanityState : uint8
{
    Awake       UMETA(DisplayName = "각성"),
    Normal      UMETA(DisplayName = "노말"),
    Fear        UMETA(DisplayName = "두려움"),
    Terror      UMETA(DisplayName = "공포"),
    Panic       UMETA(DisplayName = "패닉")
};

UENUM(BlueprintType)
enum class EOecGamePhase : uint8
{
    Phase1_MainRoom     UMETA(DisplayName = "페이즈 1: 메인 방"),
    Phase2_Corridor     UMETA(DisplayName = "페이즈 2: 복도 탐색")
};