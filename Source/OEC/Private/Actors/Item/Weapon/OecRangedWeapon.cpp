// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Item/Weapon/OecRangedWeapon.h"
#include "Data/OecDataStruct.h"
#include "Actors/Characters/Player/OecPlayerCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"

AOecRangedWeapon::AOecRangedWeapon()
{
}

void AOecRangedWeapon::InitWeaponData(const FItemStaticData& InWeaponData)
{
	Super::InitWeaponData(InWeaponData);


	MaxAmmoInClip = InWeaponData.ClipSize;

	CurrentAmmo = MaxAmmoInClip;
}

void AOecRangedWeapon::StartAttack()
{
	if (bIsReloading) return;

	// 2. 첫 발은 즉시 발사
	Fire();

	// 3. 연사 설정 (FireRate 간격으로 Fire 함수를 반복 호출)
	if (FireRate > 0.0f)
	{
		GetWorldTimerManager().SetTimer(FireTimerHandle, this, &AOecRangedWeapon::Fire, FireRate, true);
	}
}

void AOecRangedWeapon::StopAttack()
{
	GetWorldTimerManager().ClearTimer(FireTimerHandle);
}

void AOecRangedWeapon::Fire()
{
	// 1. 탄창 체크
	if (CurrentAmmo <= 0)
	{
		// TODO: 찰칵! 하는 빈 탄창 소리 재생
		return;
	}

	// 2. 총알 감소
	CurrentAmmo--;

	// 3. 총구 불꽃 & 사운드 재생
	if (FireSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
	}
	if (MuzzleFlashFX)
	{
		// WeaponMesh의 "Muzzle" 소켓에 부착해서 재생
		UNiagaraFunctionLibrary::SpawnSystemAttached(MuzzleFlashFX, WeaponMesh, TEXT("Muzzle"), FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::SnapToTarget, true);
	}

	if (!OwnerCharacter) return;

	APlayerController* pc = Cast<APlayerController>(OwnerCharacter->GetController());
	if (!pc) return;

	// ==========================================
	// 1. [논리적 발사] 데미지 판정은 무조건 '카메라(눈)' 기준!
	// ==========================================

	FVector cameraLocation;
	FRotator cameraRotation;
	pc->GetPlayerViewPoint(cameraLocation, cameraRotation);

	FVector traceStart = cameraLocation;
	FVector cameraForward = cameraRotation.Vector();

	// 카메라가 바라보는 방향을 기준으로 탄퍼짐(원뿔) 생성!
	float halfAngleRad = FMath::DegreesToRadians(SpreadAngle);
	FVector shootDirection = FMath::VRandCone(cameraForward, halfAngleRad);

	// 레이저의 끝점 (카메라 위치 + 퍼진 방향 * 사거리)
	FVector traceEnd = traceStart + (shootDirection * FireDistance);

	FHitResult hitResult;
	FCollisionQueryParams queryParams;
	queryParams.AddIgnoredActor(this);
	queryParams.AddIgnoredActor(OwnerCharacter);

	// 눈(카메라)에서부터 레이저 쏘기! 
	bool bHit = GetWorld()->LineTraceSingleByChannel(hitResult, traceStart, traceEnd, ECC_Visibility, queryParams);

	// 최종적으로 총알이 박혀야 할 진짜 목표 지점
	FVector targetPoint = bHit ? hitResult.ImpactPoint : traceEnd;

	if (bHit)
	{
		// 5. 무언가에 맞았다면! (데미지 및 스파크)
		AActor* hitActor = hitResult.GetActor();

		if (ImpactFX)
		{
			FRotator impactRotation = hitResult.ImpactNormal.Rotation();
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), ImpactFX, hitResult.ImpactPoint, impactRotation);
		}

		if (hitActor)
		{
			UGameplayStatics::ApplyDamage(hitActor, BaseDamage, pc, this, UDamageType::StaticClass());
		}
	}

	// ==========================================
	// 2. [시각적 발사] 빛줄기(Tracer)는 '총구'에서 '목표 지점'으로 이어 그리기!
	// ==========================================
	if (TracerFX && WeaponMesh)
	{
		// 이펙트의 시작점은 '진짜 총구' 위치
		FVector muzzleLocation = WeaponMesh->GetSocketLocation(TEXT("Muzzle"));

		// 총구와 최종 목표 지점 사이의 거리
		float distanceFromMuzzle = FVector::Distance(muzzleLocation, targetPoint);

		if (distanceFromMuzzle > 200.0f)
		{
			// 💡 방향: (목표 지점 - 총구 위치) = 총구에서 목표를 바라보는 방향!
			FVector traceDir = (targetPoint - muzzleLocation).GetSafeNormal();
			float bulletSpeed = 20000.0f;
			float bulletLifetime = distanceFromMuzzle / bulletSpeed;

			UNiagaraComponent* tracerComp = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), TracerFX, muzzleLocation);
			if (tracerComp)
			{
				tracerComp->SetVectorParameter(TEXT("Velocity"), traceDir * bulletSpeed);
				tracerComp->SetFloatParameter(TEXT("Lifetime"), bulletLifetime);
			}
		}
	}
	if (pc && FireCameraShakeClass)
	{
		pc->ClientStartCameraShake(FireCameraShakeClass);
	}
	if (pc)
	{
		float PitchRecoil = FMath::RandRange(MinPitchRecoil, MaxPitchRecoil);
		float YawRecoil = FMath::RandRange(-YawRecoilRange, YawRecoilRange);

		pc->AddPitchInput(PitchRecoil);
		pc->AddYawInput(YawRecoil);
	}
}

void AOecRangedWeapon::Reload()
{
	// 이미 장전 중이거나 탄창이 꽉 찼으면 무시
	if (bIsReloading || CurrentAmmo >= MaxAmmoInClip) return;

	UE_LOG(LogTemp, Log, TEXT("장전 시작..."));
	bIsReloading = true;

	// 공격 중이었다면 중단
	StopAttack();

	// 나중에 여기서 장전 애니메이션 몽타주를 재생하면 됨!
	// 일단 2초 뒤에 장전이 완료되는 것으로 시뮬레이션
	FTimerHandle reloadTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(reloadTimerHandle, [this]()
		{
			CurrentAmmo = MaxAmmoInClip;
			bIsReloading = false;
			UE_LOG(LogTemp, Log, TEXT("장전 완료! 현재 탄약: %d"), CurrentAmmo);
		}, 2.0f, false);
}

void AOecRangedWeapon::Zoom(bool bInIsZooming)
{
	if (!OwnerCharacter) return;

	if (APlayerController* pc = Cast<APlayerController>(OwnerCharacter->GetController()))
	{
		OwnerCharacter->SetAiming(bInIsZooming);

		// TODO: 조준하면 크로스헤어가 좁아지거나, 마우스 감도가 느려지는 기능도 나중에 여기에 추가하면 완벽해!
	}
}

