// Fill out your copyright notice in the Description page of Project Settings.
#include "Broom.h"
#include "BroomController.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/CapsuleComponent.h"

// Sets default values
ABroom::ABroom()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	CapsuleComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule"));
	SetRootComponent(CapsuleComp);
	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArmComp->SetupAttachment(CapsuleComp);
	SpringArmComp->TargetArmLength = 300.0f;
	SpringArmComp->bUsePawnControlRotation = true;
	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComp->SetupAttachment(SpringArmComp, USpringArmComponent::SocketName);
	CameraComp->bUsePawnControlRotation = false;
	StaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMeshComp->SetupAttachment(CapsuleComp);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("/Game/Asset/BroomStick.BroomStick"));
	if (MeshAsset.Succeeded())
	{
		StaticMeshComp->SetStaticMesh(MeshAsset.Object);
	}

}

// Called when the game starts or when spawned
void ABroom::BeginPlay()
{
	Super::BeginPlay();
	
	// �̵� ���� �ʱ�ȭ
	bIsInAir = false;
	MovementSpeed = 500.0f;
	RotationSpeed = 100.0f;
	InitialMovementSpeed = MovementSpeed;
	CurrentSpeed = MovementSpeed;

	// �߷� ���� �ʱ�ȭ
	Gravity = 980.0f;
	FallSpeed = 0.0f;
	bIsOnGround = false;

	// ������Ʈ�� ���� �ʱ�ȭ
	AirSpeed = CurrentSpeed / 2.0f;

}

void ABroom::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	bIsOnGround = IsOnGround();

	// IMC_BroomGround ����(bIsInAir == false)������ �����ϵ���
	if (!bIsInAir && !bIsOnGround)
	{
		ApplyGravity(DeltaTime);
	}
}

// �߷� ���� �Լ�
void ABroom::ApplyGravity(float DeltaTime)
{
	// �߷� ���ӵ��� ����Ͽ� FallSpeed�� ����
	FallSpeed += Gravity * DeltaTime;

	// ���� �̵� ó��
	FVector GravityOffset = FVector(0.0f, 0.0f, -FallSpeed * DeltaTime);
	AddActorWorldOffset(GravityOffset, true);

	// ���� �浹 ���� �˻�
	if (IsOnGround())
	{
		bIsOnGround = true; // ����
		FallSpeed = 0.0f;   // ���� �ӵ� �ʱ�ȭ
		UE_LOG(LogTemp, Warning, TEXT("IsOnGround?: %s"), bIsOnGround ? TEXT("true") : TEXT("false"));

	}
	else
	{
		bIsOnGround = false;
		UE_LOG(LogTemp, Warning, TEXT("IsOnGround?: %s"), bIsOnGround ? TEXT("true") : TEXT("false"));

	}
}

bool ABroom::IsOnGround()
{
	// ���� �浹 ���θ� ���� Ʈ���̽��� Ȯ��
	FVector Start = GetActorLocation();
	FVector End = Start + FVector(0.0f, 0.0f, -10.0f); // �ణ �Ʒ��� Ʈ���̽�

	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this); // �ڱ� �ڽ��� ����

	return GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params);
}
 



// Called to bind functionality to input
void ABroom::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (ABroomController* PlayerController = Cast<ABroomController>(GetController()))
		{
			if (PlayerController->MoveAction)
			{
				EnhancedInput->BindAction(
					PlayerController->MoveAction,
					ETriggerEvent::Triggered,
					this,
					&ABroom::Move
				);
			}

			if (PlayerController->LookAction)
			{
				EnhancedInput->BindAction(
					PlayerController->LookAction,
					ETriggerEvent::Triggered,
					this,
					&ABroom::Look
				);
			}

			if (PlayerController->SwitchAction)
			{
				EnhancedInput->BindAction(
					PlayerController->SwitchAction,
					ETriggerEvent::Started,
					this,
					&ABroom::Switch
				);
			}
		}
	}

}

// �̵� ȸ�� �Լ� ����
void ABroom::MoveForward(float value)
{
	if (!FMath::IsNearlyZero(value))
	{
		//FRotator CurrentRotation = GetActorRotation();
		//float AdjustedYaw = FMath::Fmod(CurrentRotation.Yaw + 360.0f, 360.0f); // 0 ~ 360 ��ȯ
		//FVector Direction = FRotationMatrix(FRotator(0.0f, AdjustedYaw, 0.0f)).GetUnitAxis(EAxis::X);

		FVector Direction = FVector::ForwardVector;
		// �α� Ȯ��
		UE_LOG(LogTemp, Warning, TEXT("MovementSpeed: %f"), CurrentSpeed);
		AddActorLocalOffset(Direction * value * CurrentSpeed * GetWorld()->DeltaTimeSeconds, true);
	}
}
void ABroom::MoveRight(float value)
{
	if (!FMath::IsNearlyZero(value))
	{
		FVector Direction = FVector::RightVector;
		AddActorLocalOffset(Direction * value * CurrentSpeed * GetWorld()->DeltaTimeSeconds, true);
	}
}
void ABroom::MoveUp(float value)
{
	if (!FMath::IsNearlyZero(value))
	{
		FVector Direction = FVector::UpVector; // Z�� �̵�
		AddActorLocalOffset(Direction * value * CurrentSpeed * GetWorld()->DeltaTimeSeconds, true);
	}
}
void ABroom::LookYaw(float value)
{
	if (!FMath::IsNearlyZero(value))
	{
		/*FRotator DeltaRotation = FRotator(0.0f, value * RotationSpeed * GetWorld()->DeltaTimeSeconds, 0.0f);
		AddActorLocalRotation(DeltaRotation);*/

		// ���� ���� ȸ���� ��������
		FRotator CurrentRotation = GetActorRotation();
		float NewYaw = CurrentRotation.Yaw + (value * RotationSpeed * GetWorld()->DeltaTimeSeconds);
		// ���ο� ȸ���� ����
		SetActorRotation(FRotator(CurrentRotation.Pitch, NewYaw, CurrentRotation.Roll));
	}
}
void ABroom::LookPitch(float value)
{
	if (!FMath::IsNearlyZero(value))
	{
		/*FRotator DeltaRotation = FRotator(value * RotationSpeed * GetWorld()->DeltaTimeSeconds,0.0f , 0.0f);
		AddActorLocalRotation(DeltaRotation);*/
		
		// ī�޶� Pitch(SpringArm�� Pitch)�� ����
		FRotator NewRotation = SpringArmComp->GetRelativeRotation();
		NewRotation.Pitch = FMath::Clamp(NewRotation.Pitch + value * RotationSpeed* GetWorld()->DeltaTimeSeconds, -80.0f, 80.0f); // RotationSpeed ���Ű� ���� �ƴϴ�, �̹��� DeltaTimeSect
		SpringArmComp->SetRelativeRotation(NewRotation);
	}
}
void ABroom::LookRoll(float value)
{
	if (!FMath::IsNearlyZero(value))
	{
		// ���� ���� ȸ���� ��������
		FRotator CurrentRotation = GetActorRotation();
		float NewRoll = CurrentRotation.Roll + (value * RotationSpeed * GetWorld()->DeltaTimeSeconds);

		// Roll ���� -90 ~ 90���� ����
		NewRoll = FMath::Clamp(NewRoll, -90.0f, 90.0f);

		// ���ο� ȸ���� ����
		SetActorRotation(FRotator(CurrentRotation.Pitch, CurrentRotation.Yaw, NewRoll));
	}
}
void ABroom::Move(const FInputActionValue& value)
{
	if (!Controller) return;

	// �Է� ��
	const FVector MoveInput = value.Get<FVector>();

	// ���� Roll �� ��������
	float CurrentRoll = GetActorRotation().Roll;
	// Roll�� ���밪�� ���� MovementSpeed�� ���������� ����
	float RollFactor = FMath::Clamp(FMath::Abs(CurrentRoll) / 180.0f, 0.0f, 1.0f);

	// ���� ��Ʈ�� ����
	CurrentSpeed = bIsOnGround ? MovementSpeed : AirSpeed;

	// CurrnetSpeed�� Roll�� ���� ���� (Roll�� �����ϸ� 1/3�� ���̱�)
	if (!FMath::IsNearlyZero(CurrentRoll))
	{
		CurrentSpeed = InitialMovementSpeed * (1.0f - (2.0f / 3.0f) * RollFactor);
	}
	else
	{
		CurrentSpeed = InitialMovementSpeed; // Roll�� ���� �� �⺻ Speed ����
	}

	// ���� ����
	if (!FMath::IsNearlyZero(MoveInput.X))
	{
		MoveForward(MoveInput.X);
	}
	if (!FMath::IsNearlyZero(MoveInput.Y))
	{
		MoveRight(MoveInput.Y);
	}

	// ���� ��忡���� Z�� �̵� ó��
	if (bIsInAir && !FMath::IsNearlyZero(MoveInput.Z))
	{
		MoveUp(MoveInput.Z);
	}
}
void ABroom::Look(const FInputActionValue& value)
{
	if (!Controller) return;

	const FVector LookInput = value.Get<FVector>();
	
	//// �����
	//FRotator CurrentRotation = GetActorRotation();
	//UE_LOG(LogTemp, Log, TEXT("Current Rotation: Pitch=%f, Yaw=%f, Roll=%f"),
	//	CurrentRotation.Pitch, CurrentRotation.Yaw, CurrentRotation.Roll);

	LookYaw(LookInput.X);
	LookPitch(LookInput.Y);

	if (bIsInAir)
	{
		LookRoll(LookInput.Z);
	}

}

void ABroom::Switch()
{
	if (ABroomController* PlayerController = Cast<ABroomController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* InputSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			if (!bIsInAir)
			{
				if (IMC_BroomFly && IMC_BroomGround)
				{
					InputSubsystem->RemoveMappingContext(IMC_BroomGround);
					InputSubsystem->AddMappingContext(IMC_BroomFly, 1);
					
				}
				bIsInAir = true; // ���� ��ȯ
				UE_LOG(LogTemp, Warning, TEXT("IsInAir?: %s"), bIsInAir ? TEXT("true") : TEXT("false"));
			}
			else
			{
				if (IMC_BroomFly && IMC_BroomGround)
				{
					InputSubsystem->RemoveMappingContext(IMC_BroomFly);
					InputSubsystem->AddMappingContext(IMC_BroomGround, 1);
				}
				bIsInAir = false; // ���� ��ȯ
				UE_LOG(LogTemp, Warning, TEXT("IsInAir?: %s"), bIsInAir ? TEXT("true") : TEXT("false"));

				FallSpeed = 0.0f; // �� �� ���� �ӵ��� �ʱ�ȭ �ؾ� �Ѵ�!
			}
		}
	}
	
}




