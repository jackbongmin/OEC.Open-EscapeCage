// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Data/OecEnumType.h"
#include "OecAnimInstance.generated.h"

/**
 * 
 */

class AOecPlayerCharacter;
class UCharacterMovementComponent;

UCLASS()
class OEC_API UOecAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float InDeltaSeconds) override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Anim")
	float Speed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Anim")
	bool bIsMoving;

	// 캐릭터 캐싱
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Anim")
	TObjectPtr<AOecPlayerCharacter> OwnerCharacter;

	// 무브먼트 컴포넌트 캐싱
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Anim")
	TObjectPtr<UCharacterMovementComponent> MovementComponent;

	// 애님 그래프에서 상체 포즈를 바꾸기 위해 사용할 무기 상태 변수
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Anim")
	EOecWeaponState CurrentWeaponState;

protected:
	// 카메라가 위아래로 꺾인 각도를 저장할 변수
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anim")
	float AimPitch;
	

};
