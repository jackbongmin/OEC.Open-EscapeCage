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
#include "Core/OecPlayerController.h"
#include "Core/OecHUD.h"
#include "UI/Ingame/OecInGameWidget.h"
#include "UI/Ingame/OecInventoryPanelWidget.h"
#include "Data/OecEnumType.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GAS/Attributeset/OecPlayerAttributeSet.h"
#include "AbilitySystemComponent.h"

AOecPlayerCharacter::AOecPlayerCharacter()
{
    PrimaryActorTick.bCanEverTick = true;

    FirstPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
    FirstPersonCamera->SetupAttachment(GetCapsuleComponent());
    FirstPersonCamera->SetRelativeLocation(FVector(0.f, 0.f, 60.f));
    FirstPersonCamera->bUsePawnControlRotation = true;

    InteractionComponent = CreateDefaultSubobject<UOecInteractionComponent>(TEXT("InteractionComponent"));

    PlayerAttributeSet = CreateDefaultSubobject<UOecPlayerAttributeSet>(TEXT("PlayerAttributeSet"));
    GetCharacterMovement()->AirControl = 0.5f;
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
    if (UAbilitySystemComponent* asc = GetAbilitySystemComponent())
    {
        PlayerAttributeSet = const_cast<UOecPlayerAttributeSet*>(asc->GetSet<UOecPlayerAttributeSet>());
    }
}

void AOecPlayerCharacter::Tick(float InDeltaTime)
{
    Super::Tick(InDeltaTime);

    HandleStamina(InDeltaTime);

    UpdateMovementSettings();
}

void AOecPlayerCharacter::SetupPlayerInputComponent(UInputComponent* InPlayerInputComponent)
{
    Super::SetupPlayerInputComponent(InPlayerInputComponent);

    if (UEnhancedInputComponent* enhancedInputComponent = Cast<UEnhancedInputComponent>(InPlayerInputComponent))
    {
        enhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AOecPlayerCharacter::Move);
        enhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AOecPlayerCharacter::Look);
        enhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this, &AOecPlayerCharacter::Interact);
        enhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Started, this, &AOecPlayerCharacter::OnSprintStarted);
        enhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &AOecPlayerCharacter::OnSprintCompleted);
        enhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &AOecPlayerCharacter::OnJumpAction);

        if (QuickSlot1Action)
        {
            enhancedInputComponent->BindAction(QuickSlot1Action, ETriggerEvent::Started, this, &AOecPlayerCharacter::ExecuteQuickSlot, 1);
            enhancedInputComponent->BindAction(QuickSlot2Action, ETriggerEvent::Started, this, &AOecPlayerCharacter::ExecuteQuickSlot, 2);
            enhancedInputComponent->BindAction(QuickSlot3Action, ETriggerEvent::Started, this, &AOecPlayerCharacter::ExecuteQuickSlot, 3);
            enhancedInputComponent->BindAction(QuickSlot4Action, ETriggerEvent::Started, this, &AOecPlayerCharacter::ExecuteQuickSlot, 4);
            enhancedInputComponent->BindAction(QuickSlot5Action, ETriggerEvent::Started, this, &AOecPlayerCharacter::ExecuteQuickSlot, 5);
        }
        if (ToggleInventoryAction)
        {
            enhancedInputComponent->BindAction(ToggleInventoryAction, ETriggerEvent::Started, this, &AOecPlayerCharacter::ToggleInventory);
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

void AOecPlayerCharacter::ToggleInventory()
{
    AOecPlayerController* pc = Cast<AOecPlayerController>(GetController());
    if (!pc) return;

    AOecHUD* hud = Cast<AOecHUD>(pc->GetHUD());
    if (!hud || !hud->GetInGameWidget()) return;

    // 최상위 위젯에서 인벤토리 패널을 가져옴
    // (주의: InGameWidget 헤더에 GetInventoryPanel() 게터 함수를 하나 만들어둬야 해!)
    UOecInventoryPanelWidget* invenPanel = hud->GetInGameWidget()->GetInventoryPanel();
    if (!invenPanel) return;

    // 현재 켜져 있는지 확인하고 반대로 뒤집기
    bool bIsVisible = invenPanel->GetVisibility() == ESlateVisibility::Visible;

    if (bIsVisible)
    {
        invenPanel->HideWidget(); // OecBaseWidget에 있는 숨기기 함수
        pc->SetUIInputMode(false); // 네가 만든 컨트롤러 함수! (게임 모드, 마우스 숨김)
    }
    else
    {
        invenPanel->ShowWidget(); // 보여주기
        pc->SetUIInputMode(true); // 네가 만든 컨트롤러 함수! (게임+UI 모드, 마우스 표시)
    }
}

void AOecPlayerCharacter::HandleStamina(float InDeltaTime)
{
    if (!PlayerAttributeSet) return;

    float curStamina = PlayerAttributeSet->GetStamina();
    float maxStamina = PlayerAttributeSet->GetMaxStamina();

    float newStamina = curStamina;

    if (bIsSprinting && GetVelocity().Size() > 10.f && !bIsExhausted)
    {
        newStamina -= 10.f * InDeltaTime;
        RegenDelayTimer = 0.f;

        if (newStamina <= 0.f)
        {
            newStamina = 0.f;
            bIsExhausted = true;
            RegenDelayTimer = 5.f; 
        }
    }
    else
    {
        if (RegenDelayTimer > 0.f)
        {
            RegenDelayTimer -= InDeltaTime;
        }
        else
        {
            bIsExhausted = false;
            if (newStamina < maxStamina)
            {
                newStamina += 15.f * InDeltaTime;
            }
        }
    }

    if (newStamina != curStamina)
    {
        PlayerAttributeSet->SetStamina(FMath::Clamp(newStamina, 0.f, maxStamina));
    }
}

void AOecPlayerCharacter::UpdateMovementSettings()
{
    if (!PlayerAttributeSet)
    {
        GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Red, TEXT("어트리뷰트 셋을 못 찾음"));
        return;
    }
    float walkSpeed = PlayerAttributeSet->GetSpeed();
    float sprintSpeed = PlayerAttributeSet->GetSprintSpeed();

    bool bCanSprint = bIsSprinting && !bIsExhausted;
    GetCharacterMovement()->MaxWalkSpeed = bCanSprint ? sprintSpeed : walkSpeed;

    GetCharacterMovement()->JumpZVelocity = PlayerAttributeSet->GetJumpForce();
    GEngine->AddOnScreenDebugMessage(1, 0.0f, FColor::Yellow, FString::Printf(TEXT("Walk: %f | Sprint: %f | IsSprinting: %d | bIsExhausted: %d"), walkSpeed, sprintSpeed, bIsSprinting, bIsExhausted));
}

void AOecPlayerCharacter::OnSprintStarted()
{
    bIsSprinting = true;
}

void AOecPlayerCharacter::OnSprintCompleted()
{
    bIsSprinting = false;
}

void AOecPlayerCharacter::OnJumpAction()
{
	Jump();
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

void AOecPlayerCharacter::SetWeaponState(EOecWeaponState InNewState)
{
    CurrentWeaponState = InNewState;
    // TODO: 나중에 여기에 진짜 무기 액터를 스폰해서 손에 쥐여주는 코드를 넣어야함
}
