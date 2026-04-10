// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Item/Weapon/OecWeaponBase.h"
#include "Actors/Characters/Player/OecPlayerCharacter.h"
#include "Components/SkeletalMeshComponent.h"
#include "Data/OecDataStruct.h"

// Sets default values
AOecWeaponBase::AOecWeaponBase()
{
	PrimaryActorTick.bCanEverTick = false;

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	RootComponent = WeaponMesh;

	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AOecWeaponBase::InitWeaponData(const FItemStaticData& InWeaponData)
{

	BaseDamage = InWeaponData.Damage;
}

void AOecWeaponBase::Equip(AOecPlayerCharacter* InOwnerCharacter, FName InSocketName)
{
	if (!InOwnerCharacter)
	{
		return;
	}
	OwnerCharacter = InOwnerCharacter;
	AttachToComponent(OwnerCharacter->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, InSocketName);
}

void AOecWeaponBase::Unequip()
{
	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	OwnerCharacter = nullptr;
}

void AOecWeaponBase::StartAttack()
{
}

void AOecWeaponBase::StopAttack()
{
}
