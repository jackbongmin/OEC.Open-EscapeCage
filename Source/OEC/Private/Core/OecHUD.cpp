// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/OecHUD.h"
#include "UI/Ingame/OecQuickSlotPanelWidget.h"
#include "Blueprint/UserWidget.h"

void AOecHUD::BeginPlay()
{
    Super::BeginPlay();

    if (QuickSlotPanelClass)
    {
        QuickSlotPanel = CreateWidget<UOecQuickSlotPanelWidget>(GetWorld(), QuickSlotPanelClass);
        if (QuickSlotPanel)
        {
            QuickSlotPanel->AddToViewport();
        }
    }
}
