// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/OecBaseWidget.h"
#include "OecQuickSlotSlotWidget.generated.h"

/**
 * 
 */

class UImage;
class UTextBlock;

UCLASS()
class OEC_API UOecQuickSlotSlotWidget : public UOecBaseWidget
{
	GENERATED_BODY()
	
public:
	void InitSlot(int32 InSlotIndex);

	void RefreshSlot(FName InItemID);

protected:
	virtual void NativeOnInitialized() override;

	UPROPERTY(meta = (BindWidget))
	UImage* ItemIcon;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* SlotIndexText;

	int32 SlotIndex;
};
