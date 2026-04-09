// Fill out your copyright notice in the Description page of Project Settings.


#include "Anim/OecAnimInstance.h"
#include "Actors/Characters/Player/OecPlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"


void UOecAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	OwnerCharacter = Cast<AOecPlayerCharacter>(TryGetPawnOwner());
	if (OwnerCharacter)
	{
		MovementComponent = OwnerCharacter->GetCharacterMovement();
	}
}

void UOecAnimInstance::NativeUpdateAnimation(float InDeltaSeconds)
{
	Super::NativeUpdateAnimation(InDeltaSeconds);

	if (OwnerCharacter && MovementComponent)
	{
		Speed = OwnerCharacter->GetVelocity().Size2D();

		bIsMoving = (Speed > 3.0f) && (MovementComponent->GetCurrentAcceleration().SizeSquared2D() > 0.0f);

		// 캐릭터가 들고 있는 무기 상태를 계속 감시해서 동기화!
		CurrentWeaponState = OwnerCharacter->CurrentWeaponState;
		
		// 캐릭터가 실제 바라보는 방향(카메라 조준 방향)
		FRotator AimRot = OwnerCharacter->GetBaseAimRotation();
		// 캐릭터 몸통의 방향
		FRotator ActorRot = OwnerCharacter->GetActorRotation();

		// 두 방향의 차이를 구하고 예쁜 각도(-180 ~ 180)로 정렬!
		FRotator DeltaRot = AimRot - ActorRot;
		DeltaRot.Normalize();

		// 위아래로 고개 숙이는 각도(Pitch)만 쏙 빼서 저장!
		AimPitch = DeltaRot.Pitch;


	}
}
