// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/Interactable/OecInteractableBase.h"
#include "Data/OecEnumType.h"
#include "OecDoorBase.generated.h"

/**
 * 
 */
UCLASS()
class OEC_API AOecDoorBase : public AOecInteractableBase
{
	GENERATED_BODY()
	
public:
	AOecDoorBase();

	UFUNCTION(BlueprintCallable, Category = "OEC|Door")
	void SetDoorState(bool bInOpen);

    UFUNCTION(BlueprintCallable, Category = "OEC|Door")
    void SetDoorLocked(bool bInLocked);

	virtual void Interact_Implementation(AActor* InInteractor) override;

protected:
    UPROPERTY(EditAnywhere, Category = "OEC|Door")
    EDoorType DoorType;

    UPROPERTY(EditAnywhere, Category = "OEC|Door")
    int32 RequiredStacks = 1;

    // 메인 문은 스택을 소모하지 않으므로, 에디터에서 체크 해제 가능하게 함
    UPROPERTY(EditAnywhere, Category = "OEC|Door")
    bool bShouldConsumeStack = true;

    UPROPERTY(BlueprintReadOnly, Category = "OEC|Door")
    bool bIsOpen = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "OEC|Door")
    bool bIsLocked = false;

    UFUNCTION(BlueprintImplementableEvent, Category = "OEC|Door")
    void BP_OnDoorStateChanged(bool bInIsOpen);

};
