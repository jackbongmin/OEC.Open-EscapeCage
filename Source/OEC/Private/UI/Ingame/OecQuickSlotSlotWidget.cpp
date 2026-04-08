// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Ingame/OecQuickSlotSlotWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Subsystems/OecGameDataSubsystem.h"

void UOecQuickSlotSlotWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	if (ItemIcon)
	{
		ItemIcon->SetVisibility(ESlateVisibility::Hidden);
	}
}
void UOecQuickSlotSlotWidget::InitSlot(int32 InSlotIndex)
{
	SlotIndex = InSlotIndex;
	if (SlotIndexText)
	{
		SlotIndexText->SetText(FText::AsNumber(SlotIndex));
		
	}
}

void UOecQuickSlotSlotWidget::RefreshSlot(FName InItemID)
{
	if (InItemID.IsNone())
	{
		if (ItemIcon)
		{
			ItemIcon->SetVisibility(ESlateVisibility::Hidden);
			return;
		}
	}
	UOecGameDataSubsystem* dataSub = GetGameInstance()->GetSubsystem<UOecGameDataSubsystem>();
	if (dataSub)
	{
		const FItemStaticData* itemData = dataSub->GetItemData(InItemID);

		// TODO: 데이터 테이블에 아이콘(Texture2D) 세팅이 완료되면 주석 풀고 사용!
		/*
		if(itemData && itemData->ItemIcon)
		{
			ItemIcon->SetBrushFromTexture(itemData->ItemIcon);
			ItemIcon->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		}
		*/

		// 임시로 작동 확인용 (아이템 있으면 보이게 처리)
		if (ItemIcon) ItemIcon->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

	}
}

