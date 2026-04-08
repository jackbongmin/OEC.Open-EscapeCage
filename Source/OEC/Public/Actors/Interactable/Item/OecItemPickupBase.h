// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/Interactable/OecInteractableBase.h"
#include "OecItemPickupBase.generated.h"

/**
 * 
 */
UCLASS()
class OEC_API AOecItemPickupBase : public AOecInteractableBase
{
	GENERATED_BODY()
	
public:
	AOecItemPickupBase();

	virtual void Interact_Implementation(AActor* InInteractor) override;

protected:
	UPROPERTY(EditAnywhere, Category = "OEC|Item")
	FName ItemCode;

	UPROPERTY(EditAnywhere, Category = "OEC|Item")
	int32 Quantity;

};
