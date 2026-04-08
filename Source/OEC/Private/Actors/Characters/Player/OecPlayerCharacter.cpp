// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Characters/Player/OecPlayerCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/OecInteractionComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Subsystems/OecQuickSlotSubsystem.h"
#include "Subsystems/OecInventorySubsystem.h"
#include "Subsystems/OecGameDataSubsystem.h"

AOecPlayerCharacter::AOecPlayerCharacter()
{
    FirstPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
    FirstPersonCamera->SetupAttachment(GetCapsuleComponent());
    FirstPersonCamera->SetRelativeLocation(FVector(0.f, 0.f, 60.f));
    FirstPersonCamera->bUsePawnControlRotation = true;

    InteractionComponent = CreateDefaultSubobject<UOecInteractionComponent>(TEXT("InteractionComponent"));
}

void AOecPlayerCharacter::BeginPlay()
{
    Super::BeginPlay();

    if (APlayerController* playerController = Cast<APlayerController>(GetController()))
    {
        if (UEnhancedInputLocalPlayerSubsystem* subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(playerController->GetLocalPlayer()))
        {
            if (DefaultMappingContext)
            {
                subsystem->AddMappingContext(DefaultMappingContext, 0);
            }
        }
    }
}

void AOecPlayerCharacter::SetupPlayerInputComponent(UInputComponent* InPlayerInputComponent)
{
    Super::SetupPlayerInputComponent(InPlayerInputComponent);

    if (UEnhancedInputComponent* enhancedInputComponent = Cast<UEnhancedInputComponent>(InPlayerInputComponent))
    {
        enhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AOecPlayerCharacter::Move);
        enhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AOecPlayerCharacter::Look);
        enhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this, &AOecPlayerCharacter::Interact);

        if (QuickSlot1Action)
        {
            enhancedInputComponent->BindAction(QuickSlot1Action, ETriggerEvent::Started, this, &AOecPlayerCharacter::ExecuteQuickSlot, 1);
            enhancedInputComponent->BindAction(QuickSlot2Action, ETriggerEvent::Started, this, &AOecPlayerCharacter::ExecuteQuickSlot, 2);
            enhancedInputComponent->BindAction(QuickSlot3Action, ETriggerEvent::Started, this, &AOecPlayerCharacter::ExecuteQuickSlot, 3);
            enhancedInputComponent->BindAction(QuickSlot4Action, ETriggerEvent::Started, this, &AOecPlayerCharacter::ExecuteQuickSlot, 4);
            enhancedInputComponent->BindAction(QuickSlot5Action, ETriggerEvent::Started, this, &AOecPlayerCharacter::ExecuteQuickSlot, 5);
        }
    }
}

void AOecPlayerCharacter::Move(const FInputActionValue& InValue)
{
    FVector2D movementVector = InValue.Get<FVector2D>();

    if (Controller != nullptr)
    {
        AddMovementInput(GetActorForwardVector(), movementVector.Y);
        AddMovementInput(GetActorRightVector(), movementVector.X);
    }
}

void AOecPlayerCharacter::Look(const FInputActionValue& InValue)
{
    FVector2D lookAxisVector = InValue.Get<FVector2D>();

    if (Controller != nullptr)
    {
        AddControllerYawInput(lookAxisVector.X);
        AddControllerPitchInput(lookAxisVector.Y);
    }
}

void AOecPlayerCharacter::Interact()
{
    if (InteractionComponent)
    {
        InteractionComponent->TryInteract();
    }
}

void AOecPlayerCharacter::ExecuteQuickSlot(int32 InSlotIndex)
{
    UOecQuickSlotSubsystem* quickSlotSub = GetGameInstance()->GetSubsystem<UOecQuickSlotSubsystem>();
    UOecInventorySubsystem* invenSub = GetGameInstance()->GetSubsystem<UOecInventorySubsystem>();
    UOecGameDataSubsystem* dataSub = GetGameInstance()->GetSubsystem<UOecGameDataSubsystem>();

    if (!quickSlotSub || !invenSub || !dataSub) return;

    // 해당 슬롯에 등록된 아이템 코드가 있는지 확인
    FName targetItemCode = quickSlotSub->GetItemAtSlot(InSlotIndex);
    if (targetItemCode == NAME_None) return; // 빈 슬롯

    // 그 아이템을 내가 진짜로 인벤토리에 가지고 있는지 확인 (수량 체크 로직 필요)
    // TODO: invenSub에 HasItem(FName InItemCode) 같은 함수 하나 만들어두면 편해!

    const FItemStaticData* itemData = dataSub->GetItemData(targetItemCode);
    if (itemData && itemData->ItemType == EItemType::Consumable)
    {
        UE_LOG(LogTemp, Log, TEXT("%d번 퀵슬롯 아이템 사용: %s"), InSlotIndex, *itemData->ItemName);

        // 여기에 GAS 어빌리티 트리거 또는 체력 회복 로직 추가!

        // 다 썼으면 인벤토리에서 1개 빼기
        invenSub->RemoveItem(targetItemCode, 1);
    }
}
