// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/OecInteractionComponent.h"
#include "GameFramework/Actor.h"
#include "Camera/CameraComponent.h"
#include "Interfaces/OecInteractableInterface.h"

UOecInteractionComponent::UOecInteractionComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

void UOecInteractionComponent::TryInteract()
{
    AActor* owner = GetOwner();
    if (!owner) return;

    UCameraComponent* camera = owner->FindComponentByClass<UCameraComponent>();
    if (!camera) return;

    FVector startLoc = camera->GetComponentLocation();
    FVector endLoc = startLoc + (camera->GetComponentRotation().Vector() * InteractRange);

    FHitResult hitResult;
    FCollisionQueryParams collisionParams;
    collisionParams.AddIgnoredActor(owner);

    bool bHit = GetWorld()->LineTraceSingleByChannel(hitResult, startLoc, endLoc, ECC_Visibility, collisionParams);

    if (bHit && hitResult.GetActor())
    {
        AActor* hitActor = hitResult.GetActor();

        if (hitActor->Implements<UOecInteractableInterface>())
        {
            IOecInteractableInterface::Execute_Interact(hitActor, owner);

            UE_LOG(LogTemp, Log, TEXT("상호작용 성공: %s"), *hitActor->GetName());
        }
    }
}

