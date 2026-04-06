// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystems/OecInventorySubsystem.h"
#include "Subsystems/OecGameDataSubsystem.h"

void UOecInventorySubsystem::Initialize(FSubsystemCollectionBase& InCollection)
{
	Super::Initialize(InCollection);
	InventorySlots.Empty();
}

bool UOecInventorySubsystem::TryAddItem(FName InItemCode, int32 InCount)
{
	// 존재하는 아이템인지 확인
	UOecGameDataSubsystem* dataSubsystem = GetGameInstance()->GetSubsystem<UOecGameDataSubsystem>();
	if(!dataSubsystem||!dataSubsystem->GetItemData(InItemCode))
	{
		UE_LOG(LogTemp, Warning, TEXT("존재하지 않는 아이템 코드: %s"), *InItemCode.ToString());
		return false;
	}

	// 이미 가지고 있는 아이템인지 확인
	for(FInventorySlot& slot : InventorySlots)
	{
		if (slot.ItemCode == InItemCode)
		{
			slot.Quantity += InCount;
			OnInventoryUpdated.Broadcast();
			return true;
		}
	}

	// 새로운 아이템 추가
	if (InventorySlots.Num() < MaxSlotCount)
	{
		FInventorySlot newSlot;
		newSlot.ItemCode = InItemCode;
		newSlot.Quantity = InCount;
		InventorySlots.Add(newSlot);

		OnInventoryUpdated.Broadcast();
		return true;
	}

	UE_LOG(LogTemp, Warning, TEXT("인벤토리가 가득 찼습니다!"));
	return false;
}

void UOecInventorySubsystem::RemoveItem(FName InItemCode, int32 InCount)
{
	for (int32 i = 0; i < InventorySlots.Num(); i++)
	{
		if (InventorySlots[i].ItemCode == InItemCode)
		{
			InventorySlots[i].Quantity -= InCount;
			// 수량이 0 이하면 슬롯 삭제
			if (InventorySlots[i].Quantity <= 0)
			{
				InventorySlots.RemoveAt(i);
			}

			OnInventoryUpdated.Broadcast();
			return;
		}
	}
}
