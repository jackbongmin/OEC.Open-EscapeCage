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

	// 💡 [변경됨] 카메라 대신 '진짜 총구(Muzzle)'의 위치와 방향을 가져옴!
	FTransform muzzleTransform = WeaponMesh->GetSocketTransform(TEXT("Muzzle"));
	FVector traceStart = muzzleTransform.GetLocation();

	// 총구가 바라보고 있는 물리적인 정면 방향 (보통 X축)
	FVector muzzleForward = muzzleTransform.GetRotation().GetForwardVector();

	// 💡 총구가 바라보는 방향을 기준으로 탄퍼짐(원뿔) 생성!
	float halfAngleRad = FMath::DegreesToRadians(SpreadAngle);
	FVector shootDirection = FMath::VRandCone(muzzleForward, halfAngleRad);

	// 레이저의 끝점 (총구 위치 + 퍼진 방향 * 사거리)
	FVector traceEnd = traceStart + (shootDirection * FireDistance);

	FHitResult hitResult;
	FCollisionQueryParams queryParams;
	queryParams.AddIgnoredActor(this); // 총기 자신 무시
	queryParams.AddIgnoredActor(OwnerCharacter); // 플레이어 무시

	// 레이저 쏘기! 
	bool bHit = GetWorld()->LineTraceSingleByChannel(hitResult, traceStart, traceEnd, ECC_Visibility, queryParams);

	FVector tracerEndPoint = bHit ? hitResult.ImpactPoint : traceEnd;

	if (bHit)
	{
		// 5. 무언가에 맞았다면! (데미지 및 스파크 이펙트는 기존과 동일)
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

	// 💡 6. 궤적 이펙트 스폰 (이제 꺾임 없이 총구에서 1자로 쭉 뻗어나감!)
	if (TracerFX && WeaponMesh)
	{
		// 이미 위에서 traceStart를 Muzzle 위치로 잡았으므로 그대로 사용
		float distanceFromMuzzle = FVector::Distance(traceStart, tracerEndPoint);

		// 2m(200.0f)보다 멀리 있을 때만 궤적 그리기
		if (distanceFromMuzzle > 200.0f)
		{
			FVector traceDir = (tracerEndPoint - traceStart).GetSafeNormal();
			float bulletSpeed = 20000.0f;
			float bulletLifetime = distanceFromMuzzle / bulletSpeed;

			UNiagaraComponent* tracerComp = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), TracerFX, traceStart);
			if (tracerComp)
			{
				tracerComp->SetVectorParameter(TEXT("Velocity"), traceDir * bulletSpeed);
				tracerComp->SetFloatParameter(TEXT("Lifetime"), bulletLifetime);
			}
		}
	}











	//// 4. 히트스캔 (레이저 발사) 로직 시작!
	//if (!OwnerCharacter) return;

	//APlayerController* pc = Cast<APlayerController>(OwnerCharacter->GetController());
	//if (!pc) return;

	//// 카메라의 현재 위치와 바라보는 방향(회전값)을 가져옴
	//FVector cameraLocation;
	//FRotator cameraRotation;
	//pc->GetPlayerViewPoint(cameraLocation, cameraRotation);

	//// 레이저의 시작점(카메라 위치)과 끝점(바라보는 방향으로 FireDistance 만큼 앞으로 간 곳) 계산
	//FVector traceStart = cameraLocation;

	//FVector shootDirection = cameraRotation.Vector();
	//float halfAngleRad = FMath::DegreesToRadians(SpreadAngle); // 각도를 라디안으로 변환
	//shootDirection = FMath::VRandCone(shootDirection, halfAngleRad); // 원뿔 형태 안에서 랜덤한 방향 겟!

	//FVector traceEnd = traceStart + (shootDirection * FireDistance);

	//FHitResult hitResult;
	//FCollisionQueryParams queryParams;
	//queryParams.AddIgnoredActor(this); // 총기 자신 무시
	//queryParams.AddIgnoredActor(OwnerCharacter); // 플레이어 무시 (안 그러면 내 뒤통수를 쏨)

	//// 레이저 쏘기! (ECC_Visibility 채널에 걸리는 물체를 찾음)
	//bool bHit = GetWorld()->LineTraceSingleByChannel(hitResult, traceStart, traceEnd, ECC_Visibility, queryParams);

	//FVector tracerEndPoint = bHit ? hitResult.ImpactPoint : traceEnd;

	//if (bHit)
	//{
	//	// 5. 무언가에 맞았다면!
	//	AActor* hitActor = hitResult.GetActor();

	//	// 맞은 위치에 스파크 불꽃(ImpactFX) 생성!
	//	if (ImpactFX)
	//	{
	//		// 맞은 표면의 법선(Normal) 방향을 바라보게 회전
	//		FRotator impactRotation = hitResult.ImpactNormal.Rotation();
	//		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), ImpactFX, hitResult.ImpactPoint, impactRotation);
	//	}

	//	// 적에게 데미지 입히기 (BaseDamage는 부모 클래스에 있던 변수!)
	//	if (hitActor)
	//	{
	//		UGameplayStatics::ApplyDamage(hitActor, BaseDamage, pc, this, UDamageType::StaticClass());
	//		UE_LOG(LogTemp, Warning, TEXT("명중! 대상: %s / 데미지: %f"), *hitActor->GetName(), BaseDamage);
	//	}
	//}

	//if (TracerFX && WeaponMesh)
	//{

	//	FVector muzzleLocation = WeaponMesh->GetSocketLocation(TEXT("Muzzle"));

	//	// 총구(Muzzle)와 목표 지점(HitPoint) 사이의 '실제 거리'를 계산해!
	//	float distanceFromMuzzle = FVector::Distance(muzzleLocation, tracerEndPoint);

	//	// 💡 [핵심 해결법] 목표가 2m(200.0f)보다 멀리 있을 때만 궤적을 그린다!
	//	// (가까운 벽은 궤적이 기괴하게 꺾이거나 역주행하므로 아예 안 그리는 게 자연스러움)
	//	if (distanceFromMuzzle > 200.0f)
	//	{
	//		FVector traceDir = (tracerEndPoint - muzzleLocation).GetSafeNormal();
	//		float bulletSpeed = 20000.0f;
	//		float bulletLifetime = distanceFromMuzzle / bulletSpeed;

	//		UNiagaraComponent* tracerComp = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), TracerFX, muzzleLocation);
	//		if (tracerComp)
	//		{
	//			tracerComp->SetVectorParameter(TEXT("Velocity"), traceDir * bulletSpeed);
	//			tracerComp->SetFloatParameter(TEXT("Lifetime"), bulletLifetime);
	//		}
	//	}











		//FVector muzzleLocation = WeaponMesh->GetSocketLocation(TEXT("Muzzle"));

		//// 1. 총알이 날아갈 방향과 거리 계산
		//FVector traceDir = (tracerEndPoint - muzzleLocation).GetSafeNormal();
		//float distance = FVector::Distance(muzzleLocation, tracerEndPoint);

		//// 2. 총알 속도 설정 (원하는 대로 조절해! 20000.f 정도면 눈에 확 띔)
		//float bulletSpeed = 20000.0f;

		//// 3. 총알이 목표에 도달하는 시간(수명) 계산 (거리 / 속력)
		//float bulletLifetime = distance / bulletSpeed;

		//// 나이아가라 스폰!
		//UNiagaraComponent* tracerComp = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), TracerFX, muzzleLocation);
		//if (tracerComp)
		//{
		//	// 나이아가라에 '날아갈 속도(Velocity)'와 '살아있을 시간(Lifetime)'을 전달!
		//	tracerComp->SetVectorParameter(TEXT("Velocity"), traceDir * bulletSpeed);
		//	tracerComp->SetFloatParameter(TEXT("Lifetime"), bulletLifetime);
		//}


		//FVector muzzleLocation = WeaponMesh->GetSocketLocation(TEXT("Muzzle"));

		//// 총구 위치에서 이펙트를 생성함
		//UNiagaraComponent* tracerComp = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), TracerFX, muzzleLocation);
		//if (tracerComp)
		//{
		//	// 나이아가라 빔(Beam) 이펙트의 끝점을 설정!
		//	// (주의: 에디터 나이아가라 시스템에서 받는 변수 이름이 "BeamEnd" 인지 확인 필요)
		//	tracerComp->SetVectorParameter(TEXT("BeamEnd"), tracerEndPoint);
		//}
	//}

	// [디버그용] 실제로 레이저가 어떻게 날아갔는지 빨간 선으로 그려줌! (확인 끝나면 지워도 됨)
	//DrawDebugLine(GetWorld(), traceStart, bHit ? hitResult.ImpactPoint : traceEnd, FColor::Red, false, 2.0f, 0, 1.0f);
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

