// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/OecInteractableInterface.h"
#include "OecInteractableBase.generated.h"

UCLASS()
class OEC_API AOecInteractableBase : public AActor, public IOecInteractableInterface
{
	GENERATED_BODY()
	
public:
    AOecInteractableBase();

    virtual void Interact_Implementation(AActor* InInteractor) override;

protected:

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "OEC|Components")
    UStaticMeshComponent* MeshComponent;
};
