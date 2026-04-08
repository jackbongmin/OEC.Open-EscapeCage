// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "OecHUD.generated.h"

/**
 * 
 */

class UOecQuickSlotPanelWidget;

UCLASS()
class OEC_API AOecHUD : public AHUD
{
	GENERATED_BODY()
	
public:
	virtual void BeginPlay() override;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "OEC|UI")
	TSubclassOf<UOecQuickSlotPanelWidget> QuickSlotPanelClass;

	UPROPERTY()
	UOecQuickSlotPanelWidget* QuickSlotPanel;
};
