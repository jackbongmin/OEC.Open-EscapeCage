// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Attributeset/OecAttributeSetBase.h"

UOecAttributeSetBase::UOecAttributeSetBase()
{
    InitHealth(100.f);
    InitMaxHealth(100.f);
    InitAttackPower(10.f);
    InitSpeed(600.f);
}

void UOecAttributeSetBase::PreAttributeChange(const FGameplayAttribute& InAttribute, float& InNewValue)
{
}
