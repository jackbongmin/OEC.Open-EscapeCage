// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/OecBaseWidget.h"
#include "OecInGameWidget.generated.h"

/**
 * 
 */

class UOecQuickSlotPanelWidget;

UCLASS()
class OEC_API UOecInGameWidget : public UOecBaseWidget
{
	GENERATED_BODY()
	
public:
	UOecQuickSlotPanelWidget* GetQuickSlotPanel() const { return QuickSlotPanel; }

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	UOecQuickSlotPanelWidget* QuickSlotPanel;
};
