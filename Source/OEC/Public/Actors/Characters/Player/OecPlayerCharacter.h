// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/Characters/OecCharacterBase.h"
#include "OecPlayerCharacter.generated.h"

/**
 * 
 */

class UCameraComponent;
class UOecInteractionComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

UCLASS()
class OEC_API AOecPlayerCharacter : public AOecCharacterBase
{
	GENERATED_BODY()
	
public:
	AOecPlayerCharacter();

protected:
	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Enhanced Input
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void Interact();

	void ToggleInventory();

private:
    // 1인칭 카메라
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "OEC|Camera", meta = (AllowPrivateAccess = "true"))
    UCameraComponent* FirstPersonCamera;

    // 상호작용 (레이저 쏘기)
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "OEC|Interaction", meta = (AllowPrivateAccess = "true"))
    UOecInteractionComponent* InteractionComponent;

    // Enhanced Input 에셋 슬롯
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "OEC|Input", meta = (AllowPrivateAccess = "true"))
    UInputMappingContext* DefaultMappingContext;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "OEC|Input", meta = (AllowPrivateAccess = "true"))
    UInputAction* MoveAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "OEC|Input", meta = (AllowPrivateAccess = "true"))
    UInputAction* LookAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "OEC|Input", meta = (AllowPrivateAccess = "true"))
    UInputAction* InteractAction;


public:
    void ExecuteQuickSlot(int32 InSlotIndex);

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "OEC|Input")
    UInputAction* ToggleInventoryAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "OEC|Input")
    UInputAction* QuickSlot1Action;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "OEC|Input")
    UInputAction* QuickSlot2Action;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "OEC|Input")
    UInputAction* QuickSlot3Action;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "OEC|Input")
    UInputAction* QuickSlot4Action;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "OEC|Input")
    UInputAction* QuickSlot5Action;


};
