// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Attributeset/OecPlayerAttributeSet.h"

UOecPlayerAttributeSet::UOecPlayerAttributeSet()
{
    InitSanity(100.f);
    InitAnswerCount(0.f);

    InitStamina(100.f);
    InitMaxStamina(100.f);

    InitSprintSpeed(1500.f);
    InitJumpForce(600.f);
}
