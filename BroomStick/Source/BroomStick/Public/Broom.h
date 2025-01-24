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

	//������Ʈ �߰�
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Broom|Component")
	UCapsuleComponent* CapsuleComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Broom|Component")
	UStaticMeshComponent* StaticMeshComp;

	//�̵� ȸ�� �Լ� ����
	void MoveForward(float value);
	void MoveRight(float value);
	void MoveUp(float value);

	void LookYaw(float value);
	void LookPitch(float value);
	void LookRoll(float value);

	// Input Mapping Context (IMC) ����
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputMappingContext* IMC_BroomFly;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputMappingContext* IMC_BroomGround;

	// �̵��ӵ�, ȸ���ӵ� ����
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float MovementSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float RotationSpeed;

	float CurrentSpeed;
	// �߷�, ���� ����
	// �߷� ���
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gravity")
	float Gravity; // Unreal Engine �⺻ �߷� (cm/s^2)

	// ���� �ӵ�
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Gravity")
	float FallSpeed;

	// ���� ����
	bool bIsOnGround;

	// ���� ��Ʈ�� ����
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
	
	// �߷� ���� �Լ�
	void ApplyGravity(float DeltaTime);
	bool IsOnGround();

private:
	bool bIsInAir;
	float InitialMovementSpeed;
	

};
