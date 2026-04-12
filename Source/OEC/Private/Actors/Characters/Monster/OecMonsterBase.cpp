// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Characters/Monster/OecMonsterBase.h"
#include "AbilitySystemComponent.h"
#include "GAS/Attributeset/OecAttributeSetBase.h"

#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"

AOecMonsterBase::AOecMonsterBase()
{
	AttributeSet = CreateDefaultSubobject<UOecAttributeSetBase>(TEXT("AttributeSet"));

	if (GetCapsuleComponent())
	{
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

		GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	}

	if (GetMesh())
	{
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	}
}

void AOecMonsterBase::BeginPlay()
{
	Super::BeginPlay();

	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->InitAbilityActorInfo(this, this);

		AbilitySystemComponent->SetNumericAttributeBase(UOecAttributeSetBase::GetHealthAttribute(), 100.0f);
		AbilitySystemComponent->SetNumericAttributeBase(UOecAttributeSetBase::GetMaxHealthAttribute(), 100.0f);
	
		AbilitySystemComponent->AddLooseGameplayTag(FGameplayTag::RequestGameplayTag(FName("Character.Monster")));
	}
}
