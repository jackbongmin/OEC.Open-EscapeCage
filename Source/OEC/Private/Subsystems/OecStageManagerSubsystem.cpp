// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystems/OecStageManagerSubsystem.h"

void UOecStageManagerSubsystem::Initialize(FSubsystemCollectionBase& InCollection)
{
	Super::Initialize(InCollection);

	CurrentStack = 0;
	MaxStack = 5;
	CurrentPhase = EOecGamePhase::Phase1_MainRoom;
}

void UOecStageManagerSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

void UOecStageManagerSubsystem::AddStack(int32 InAmount)
{
	CurrentStack = FMath::Clamp(CurrentStack + InAmount, 0, MaxStack);
	OnStackChanged.Broadcast(CurrentStack, MaxStack);

	if(CurrentStack>=MaxStack && CurrentPhase == EOecGamePhase::Phase1_MainRoom)
	{
		UE_LOG(LogTemp, Warning, TEXT("페이즈 2 진입"));
		ChangePhase(EOecGamePhase::Phase2_Corridor);
	}
}

void UOecStageManagerSubsystem::ChangePhase(EOecGamePhase InNewPhase)
{
	CurrentPhase = InNewPhase;

	OnPhaseChanged.Broadcast();

	// 페이즈에 따른 방 생성
	if (CurrentPhase == EOecGamePhase::Phase2_Corridor)
	{
		GeneratePhase2Rooms();
	}
}

void UOecStageManagerSubsystem::GeneratePhase2Rooms()
{
	UE_LOG(LogTemp, Log, TEXT("페이즈 2: 복도 방 랜덤 생성 완료"));
}
