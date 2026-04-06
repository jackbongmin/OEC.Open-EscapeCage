// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Interactable/OecInteractableBase.h"

// Sets default values
AOecInteractableBase::AOecInteractableBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

    MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
    RootComponent = MeshComponent;

    // 레이저(LineTrace)에 맞을 수 있도록 콜리전 세팅 (ECC_Visibility 채널에 반응)
    MeshComponent->SetCollisionObjectType(ECC_WorldDynamic);
    MeshComponent->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
}

void AOecInteractableBase::Interact_Implementation(AActor* InInteractor)
{
    UE_LOG(LogTemp, Log, TEXT("[%s] 기본 상호작용 호출됨!"), *GetName());
}

