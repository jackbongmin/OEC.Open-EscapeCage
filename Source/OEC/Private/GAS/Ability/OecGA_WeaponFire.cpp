// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/Ability/OecGA_WeaponFire.h"
#include "Actors/Item/Weapon/OecRangedWeapon.h"
#include "Actors/Characters/Player/OecPlayerCharacter.h"
#include "AbilitySystemComponent.h"
#include "Kismet/GameplayStatics.h"


UOecGA_WeaponFire::UOecGA_WeaponFire()
{
	AbilityInputID = EOecAbilityInputID::Fire;
}

bool UOecGA_WeaponFire::CheckCost(const FGameplayAbilitySpecHandle InHandle, const FGameplayAbilityActorInfo* InActorInfo, OUT FGameplayTagContainer* OutOptionalRelevantTags) const
{
	if (!Super::CheckCost(InHandle, InActorInfo, OutOptionalRelevantTags)) return false;

	AOecRangedWeapon* weapon = Cast<AOecRangedWeapon>(GetCurrentSourceObject());
	if (weapon && weapon->GetCurrentAmmo() > 0)
	{
		return true;
	}

	return false;
}

void UOecGA_WeaponFire::ApplyCost(const FGameplayAbilitySpecHandle InHandle, const FGameplayAbilityActorInfo* InActorInfo, const FGameplayAbilityActivationInfo InActivationInfo) const
{
	Super::ApplyCost(InHandle, InActorInfo, InActivationInfo);

	AOecRangedWeapon* weapon = Cast<AOecRangedWeapon>(GetCurrentSourceObject());
	if (weapon)
	{
		weapon->ConsumeAmmo(1);
	}
}

void UOecGA_WeaponFire::ActivateAbility(const FGameplayAbilitySpecHandle InHandle, const FGameplayAbilityActorInfo* InActorInfo, const FGameplayAbilityActivationInfo InActivationInfo, const FGameplayEventData* InTriggerEventData)
{
	Super::ActivateAbility(InHandle, InActorInfo, InActivationInfo, InTriggerEventData);

	if (!CommitAbility(InHandle, InActorInfo, InActivationInfo))
	{
		EndAbility(InHandle, InActorInfo, InActivationInfo, true, true);
		return;
	}

	// 1. 발사 승인이 났으니 무기와 플레이어 정보 가져오기
	AOecRangedWeapon* weapon = Cast<AOecRangedWeapon>(GetCurrentSourceObject());
	AOecPlayerCharacter* player = Cast<AOecPlayerCharacter>(InActorInfo->AvatarActor.Get());

	if (!weapon || !player || !DamageEffectClass)
	{
		EndAbility(InHandle, InActorInfo, InActivationInfo, true, true);
		return;
	}

	// 2. 시각적 연출 실행 (총구 불꽃, 소리, 카메라 반동)
	weapon->PlayFireFX();

	// 3. 레이저 발사 (눈 기준)
	APlayerController* pc = Cast<APlayerController>(player->GetController());
	if (pc)
	{
		FVector cameraLocation;
		FRotator cameraRotation;
		pc->GetPlayerViewPoint(cameraLocation, cameraRotation);

		FVector traceStart = cameraLocation;
		FVector shootDirection = FMath::VRandCone(cameraRotation.Vector(), FMath::DegreesToRadians(weapon->SpreadAngle));
		FVector traceEnd = traceStart + (shootDirection * weapon->GetFireDistance());

		FHitResult hitResult;
		FCollisionQueryParams queryParams;
		queryParams.AddIgnoredActor(player);
		queryParams.AddIgnoredActor(weapon);

		bool bHit = GetWorld()->LineTraceSingleByChannel(hitResult, traceStart, traceEnd, ECC_Visibility, queryParams);

		weapon->PlayHitAndTracerFX(hitResult, traceEnd, bHit);

		if (bHit)
		{
			// 타겟 확인 및 데미지 GE 적용
			if (IAbilitySystemInterface* targetInterface = Cast<IAbilitySystemInterface>(hitResult.GetActor()))
			{
				UAbilitySystemComponent* targetASC = targetInterface->GetAbilitySystemComponent();
				UAbilitySystemComponent* sourceASC = InActorInfo->AbilitySystemComponent.Get();

				if (targetASC && sourceASC)
				{
					FGameplayEffectContextHandle contextHandle = sourceASC->MakeEffectContext();
					FGameplayEffectSpecHandle specHandle = sourceASC->MakeOutgoingSpec(DamageEffectClass, 1.0f, contextHandle);

					if (specHandle.IsValid())
					{
						// 무기 베이스 데미지 전송!
						specHandle.Data.Get()->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag(FName("Data.Damage")), -weapon->GetBaseDamage());
						sourceASC->ApplyGameplayEffectSpecToTarget(*specHandle.Data.Get(), targetASC);
					}
				}
			}
		}
	}

	// 4. 발사 한 번 끝났으니 어빌리티 종료
	EndAbility(InHandle, InActorInfo, InActivationInfo, true, false);
}
