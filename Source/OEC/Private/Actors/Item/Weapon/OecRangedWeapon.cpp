// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Item/Weapon/OecRangedWeapon.h"
#include "Data/OecDataStruct.h"
#include "Actors/Characters/Player/OecPlayerCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "NiagaraFunctionLibrary.h"

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

	// 4. 히트스캔 (레이저 발사) 로직 시작!
	if (!OwnerCharacter) return;

	APlayerController* pc = Cast<APlayerController>(OwnerCharacter->GetController());
	if (!pc) return;

	// 카메라의 현재 위치와 바라보는 방향(회전값)을 가져옴
	FVector cameraLocation;
	FRotator cameraRotation;
	pc->GetPlayerViewPoint(cameraLocation, cameraRotation);

	// 레이저의 시작점(카메라 위치)과 끝점(바라보는 방향으로 FireDistance 만큼 앞으로 간 곳) 계산
	FVector traceStart = cameraLocation;
	FVector traceEnd = traceStart + (cameraRotation.Vector() * FireDistance);

	FHitResult hitResult;
	FCollisionQueryParams queryParams;
	queryParams.AddIgnoredActor(this); // 총기 자신 무시
	queryParams.AddIgnoredActor(OwnerCharacter); // 플레이어 무시 (안 그러면 내 뒤통수를 쏨)

	// 레이저 쏘기! (ECC_Visibility 채널에 걸리는 물체를 찾음)
	bool bHit = GetWorld()->LineTraceSingleByChannel(hitResult, traceStart, traceEnd, ECC_Visibility, queryParams);

	if (bHit)
	{
		// 5. 무언가에 맞았다면!
		AActor* hitActor = hitResult.GetActor();

		// 맞은 위치에 스파크 불꽃(ImpactFX) 생성!
		if (ImpactFX)
		{
			// 맞은 표면의 법선(Normal) 방향을 바라보게 회전
			FRotator impactRotation = hitResult.ImpactNormal.Rotation();
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), ImpactFX, hitResult.ImpactPoint, impactRotation);
		}

		// 적에게 데미지 입히기 (BaseDamage는 부모 클래스에 있던 변수!)
		if (hitActor)
		{
			UGameplayStatics::ApplyDamage(hitActor, BaseDamage, pc, this, UDamageType::StaticClass());
			UE_LOG(LogTemp, Warning, TEXT("명중! 대상: %s / 데미지: %f"), *hitActor->GetName(), BaseDamage);
		}
	}

	// [디버그용] 실제로 레이저가 어떻게 날아갔는지 빨간 선으로 그려줌! (확인 끝나면 지워도 됨)
	DrawDebugLine(GetWorld(), traceStart, bHit ? hitResult.ImpactPoint : traceEnd, FColor::Red, false, 2.0f, 0, 1.0f);
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

}

