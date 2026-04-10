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

#include "Actors/Item/Weapon/OecWeaponBase.h"
#include "Data/OecDataStruct.h"

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
        if (FireAction)
        {
            enhancedInputComponent->BindAction(FireAction, ETriggerEvent::Started, this, &AOecPlayerCharacter::OnFireStarted);
            enhancedInputComponent->BindAction(FireAction, ETriggerEvent::Completed, this, &AOecPlayerCharacter::OnFireCompleted);
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

void AOecPlayerCharacter::OnFireStarted()
{
    if (CurrentWeaponActor)
    {
        CurrentWeaponActor->StartAttack();
    }
}

void AOecPlayerCharacter::OnFireCompleted()
{
    if (CurrentWeaponActor)
    {
        CurrentWeaponActor->StopAttack();
    }
}

void AOecPlayerCharacter::ExecuteQuickSlot(int32 InSlotIndex)
{
    UOecQuickSlotSubsystem* quickSlotSub = GetGameInstance()->GetSubsystem<UOecQuickSlotSubsystem>();
    UOecInventorySubsystem* invenSub = GetGameInstance()->GetSubsystem<UOecInventorySubsystem>();
    UOecGameDataSubsystem* dataSub = GetGameInstance()->GetSubsystem<UOecGameDataSubsystem>();

    if (!quickSlotSub || !invenSub || !dataSub) return;

    // 1. 해당 슬롯의 아이템 코드 가져오기
    FName targetItemCode = quickSlotSub->GetItemAtSlot(InSlotIndex);
    if (targetItemCode == NAME_None) return;

    // 2. 데이터 테이블에서 상세 정보 찾기
    const FItemStaticData* itemData = dataSub->GetItemData(targetItemCode);
    if (!itemData) return;

    // 3. 타입에 따라 행동 결정
    if (itemData->ItemType == EItemType::Consumable)
    {
        // 소비템: 사용하고 개수 줄이기
        UE_LOG(LogTemp, Log, TEXT("%d번 슬롯 소비템 사용: %s"), InSlotIndex, *itemData->ItemName);
        invenSub->RemoveItem(targetItemCode, 1);
    }
    else if (itemData->ItemType == EItemType::Weapon)
    {
        // 무기: 장착 프로세스 시작!
        UE_LOG(LogTemp, Log, TEXT("%d번 슬롯 무기 장착: %s"), InSlotIndex, *itemData->ItemName);

        // 데이터 테이블에 적힌 애니메이션 상태(Rifle, Pistol 등)와 아이템 코드를 넘김
        SetWeaponState(itemData->WeaponAnimState, targetItemCode);
    }
}

void AOecPlayerCharacter::SetWeaponState(EOecWeaponState InNewState, FName InItemCode)
{
    UE_LOG(LogTemp, Warning, TEXT("[SetWeaponState] 1. 함수 진입! 상태: %d, 아이템코드: %s"), (int32)InNewState, *InItemCode.ToString());

    CurrentWeaponState = InNewState;
    
     //1. 기존에 들고 있던 무기가 있다면 파괴 (또는 풀링 반납)
    if (CurrentWeaponActor)
    {
        UE_LOG(LogTemp, Warning, TEXT("[SetWeaponState] 1-1. 기존 무기 파괴함"));
        CurrentWeaponActor->Destroy();
        CurrentWeaponActor = nullptr;
    }
    // 2. 맨손이면 여기서 종료
    if (CurrentWeaponState == EOecWeaponState::None || InItemCode == NAME_None)
    {
        UE_LOG(LogTemp, Error, TEXT("[SetWeaponState] 2. 맨손 상태이거나 아이템 코드가 없어서 취소됨!"));
        return;
    }

    // 3. 게임 데이터 서브시스템에서 아이템 정보(데이터 테이블) 긁어오기
    UOecGameDataSubsystem* dataSub = GetGameInstance()->GetSubsystem<UOecGameDataSubsystem>();
    if (!dataSub)
    {
        UE_LOG(LogTemp, Error, TEXT("[SetWeaponState] 3. 데이터 서브시스템을 찾을 수 없음!"));
        return;
    }

    const FItemStaticData* itemData = dataSub->GetItemData(InItemCode);
    if (!itemData)
    {
        UE_LOG(LogTemp, Error, TEXT("[SetWeaponState] 4. 데이터 테이블에서 해당 아이템을 찾을 수 없음!"));
        return;
    }

    // 5. 클래스 유효성 확인 (가장 의심되는 부분!!)
    if (itemData->ItemActorClass.IsNull())
    {
        UE_LOG(LogTemp, Error, TEXT("[SetWeaponState] 5. 데이터 테이블에 스폰할 무기 클래스(ItemActorClass)가 비어있음!"));
        return;
    }

    UClass* WeaponClassToSpawn = itemData->ItemActorClass.LoadSynchronous();
    if (!WeaponClassToSpawn)
    {
        UE_LOG(LogTemp, Error, TEXT("[SetWeaponState] 6. 무기 클래스를 메모리에 로드하는데 실패함!"));
        return;
    }

    UE_LOG(LogTemp, Warning, TEXT("[SetWeaponState] 7. 무기 스폰 직전! 클래스: %s"), *WeaponClassToSpawn->GetName());

    FActorSpawnParameters spawnParams;
    spawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    AOecWeaponBase* spawnedWeapon = GetWorld()->SpawnActor<AOecWeaponBase>(WeaponClassToSpawn, GetActorLocation(), GetActorRotation(), spawnParams);

    if (spawnedWeapon)
    {
        UE_LOG(LogTemp, Warning, TEXT("[SetWeaponState] 8. 스폰 성공! 데이터 주입 및 장착 진행"));
        spawnedWeapon->InitWeaponData(*itemData);
        spawnedWeapon->Equip(this, TEXT("HandGrip_R"));
        CurrentWeaponActor = spawnedWeapon;
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("[SetWeaponState] 9. SpawnActor 자체가 실패함!"));
    }
}
