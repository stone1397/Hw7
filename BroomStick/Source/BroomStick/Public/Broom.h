// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Broom.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UCapsuleComponent;
struct FInputActionValue;
class UInputMappingContext;

UCLASS()
class BROOMSTICK_API ABroom : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ABroom();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	USpringArmComponent* SpringArmComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	UCameraComponent* CameraComp;

	//컴포넌트 추가
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Broom|Component")
	UCapsuleComponent* CapsuleComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Broom|Component")
	UStaticMeshComponent* StaticMeshComp;

	//이동 회전 함수 구현
	void MoveForward(float value);
	void MoveRight(float value);
	void MoveUp(float value);

	void LookYaw(float value);
	void LookPitch(float value);
	void LookRoll(float value);

	// Input Mapping Context (IMC) 참조
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputMappingContext* IMC_BroomFly;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputMappingContext* IMC_BroomGround;

	// 이동속도, 회전속도 정의
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float MovementSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float RotationSpeed;

	float CurrentSpeed;
	// 중력, 낙하 관련
	// 중력 상수
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gravity")
	float Gravity; // Unreal Engine 기본 중력 (cm/s^2)

	// 낙하 속도
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gravity")
	float FallSpeed;

	// 착지 여부
	bool bIsOnGround;

	// 에어 컨트롤 관련
	float AirSpeed;


public:	
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION()
	void Move(const FInputActionValue& value);
	UFUNCTION()
	void Look(const FInputActionValue& value);
	UFUNCTION()
	void Switch();
	
	// 중력 관련 함수
	void ApplyGravity(float DeltaTime);
	bool IsOnGround();

private:
	bool bIsInAir;
	float InitialMovementSpeed;
	

};
