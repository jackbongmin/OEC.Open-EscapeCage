// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/OecPlayerController.h"

void AOecPlayerController::BeginPlay()
{
	Super::BeginPlay();

	SetUIInputMode(false);
}
void AOecPlayerController::SetUIInputMode(bool InIsUIOnly)
{
	if (InIsUIOnly)
	{
		FInputModeGameAndUI inputMode;
		inputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		SetInputMode(inputMode);
		bShowMouseCursor = true;
	}
	else
	{
		FInputModeGameOnly inputMode;
		SetInputMode(inputMode);
		bShowMouseCursor = false;
	}
}

