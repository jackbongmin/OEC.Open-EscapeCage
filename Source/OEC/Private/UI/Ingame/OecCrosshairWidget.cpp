// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Ingame/OecCrosshairWidget.h"
#include "Components/Image.h"
#include "Components/CanvasPanel.h"
#include "Animation/WidgetAnimation.h"

void UOecCrosshairWidget::OnWeaponStateChanged(bool bIsArmed)
{
	if (DotImage && CrossCanvas)
	{
		if (bIsArmed)
		{
			// 무기 장착: 점 숨기고, 십자선 켜기
			DotImage->SetVisibility(ESlateVisibility::Hidden);
			CrossCanvas->SetVisibility(ESlateVisibility::SelfHitTestInvisible); 
		}
		else
		{
			// 맨손: 점 켜고, 십자선 숨기기
			DotImage->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			CrossCanvas->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

void UOecCrosshairWidget::OnFireWeapon()
{
	if (Anim_Fire)
	{
		PlayAnimation(Anim_Fire);
	}
}
