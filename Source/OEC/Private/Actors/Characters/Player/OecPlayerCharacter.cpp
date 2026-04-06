// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Characters/Player/OecPlayerCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/OecInteractionComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

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
