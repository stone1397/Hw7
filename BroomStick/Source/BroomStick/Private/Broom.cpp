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
	
	// 이동 관련 초기화
	bIsInAir = false;
	MovementSpeed = 500.0f;
	RotationSpeed = 100.0f;
	InitialMovementSpeed = MovementSpeed;
	CurrentSpeed = MovementSpeed;

	// 중력 관련 초기화
	Gravity = 980.0f;
	FallSpeed = 0.0f;
	bIsOnGround = false;

	// 에어컨트롤 관련 초기화
	AirSpeed = CurrentSpeed / 2.0f;

}

void ABroom::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	bIsOnGround = IsOnGround();

	// IMC_BroomGround 상태(bIsInAir == false)에서만 낙하하도록
	if (!bIsInAir && !bIsOnGround)
	{
		ApplyGravity(DeltaTime);
	}
}

// 중력 관련 함수
void ABroom::ApplyGravity(float DeltaTime)
{
	// 중력 가속도를 계산하여 FallSpeed를 증가
	FallSpeed += Gravity * DeltaTime;

	// 낙하 이동 처리
	FVector GravityOffset = FVector(0.0f, 0.0f, -FallSpeed * DeltaTime);
	AddActorWorldOffset(GravityOffset, true);

	// 지면 충돌 여부 검사
	if (IsOnGround())
	{
		bIsOnGround = true; // 착지
		FallSpeed = 0.0f;   // 낙하 속도 초기화
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
	// 지면 충돌 여부를 라인 트레이스로 확인
	FVector Start = GetActorLocation();
	FVector End = Start + FVector(0.0f, 0.0f, -10.0f); // 약간 아래로 트레이스

	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this); // 자기 자신은 무시

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

// 이동 회전 함수 정의
void ABroom::MoveForward(float value)
{
	if (!FMath::IsNearlyZero(value))
	{
		//FRotator CurrentRotation = GetActorRotation();
		//float AdjustedYaw = FMath::Fmod(CurrentRotation.Yaw + 360.0f, 360.0f); // 0 ~ 360 변환
		//FVector Direction = FRotationMatrix(FRotator(0.0f, AdjustedYaw, 0.0f)).GetUnitAxis(EAxis::X);

		FVector Direction = FVector::ForwardVector;
		// 로그 확인
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
		FVector Direction = FVector::UpVector; // Z축 이동
		AddActorLocalOffset(Direction * value * CurrentSpeed * GetWorld()->DeltaTimeSeconds, true);
	}
}
void ABroom::LookYaw(float value)
{
	if (!FMath::IsNearlyZero(value))
	{
		/*FRotator DeltaRotation = FRotator(0.0f, value * RotationSpeed * GetWorld()->DeltaTimeSeconds, 0.0f);
		AddActorLocalRotation(DeltaRotation);*/

		// 현재 로컬 회전값 가져오기
		FRotator CurrentRotation = GetActorRotation();
		float NewYaw = CurrentRotation.Yaw + (value * RotationSpeed * GetWorld()->DeltaTimeSeconds);
		// 새로운 회전값 적용
		SetActorRotation(FRotator(CurrentRotation.Pitch, NewYaw, CurrentRotation.Roll));
	}
}
void ABroom::LookPitch(float value)
{
	if (!FMath::IsNearlyZero(value))
	{
		/*FRotator DeltaRotation = FRotator(value * RotationSpeed * GetWorld()->DeltaTimeSeconds,0.0f , 0.0f);
		AddActorLocalRotation(DeltaRotation);*/
		
		// 카메라 Pitch(SpringArm의 Pitch)를 조정
		FRotator NewRotation = SpringArmComp->GetRelativeRotation();
		NewRotation.Pitch = FMath::Clamp(NewRotation.Pitch + value * RotationSpeed* GetWorld()->DeltaTimeSeconds, -80.0f, 80.0f); // RotationSpeed 제거가 답은 아니다, 이번엔 DeltaTimeSect
		SpringArmComp->SetRelativeRotation(NewRotation);
	}
}
void ABroom::LookRoll(float value)
{
	if (!FMath::IsNearlyZero(value))
	{
		// 현재 로컬 회전값 가져오기
		FRotator CurrentRotation = GetActorRotation();
		float NewRoll = CurrentRotation.Roll + (value * RotationSpeed * GetWorld()->DeltaTimeSeconds);

		// Roll 값을 -90 ~ 90도로 제한
		NewRoll = FMath::Clamp(NewRoll, -90.0f, 90.0f);

		// 새로운 회전값 적용
		SetActorRotation(FRotator(CurrentRotation.Pitch, CurrentRotation.Yaw, NewRoll));
	}
}
void ABroom::Move(const FInputActionValue& value)
{
	if (!Controller) return;

	// 입력 값
	const FVector MoveInput = value.Get<FVector>();

	// 현재 Roll 값 가져오기
	float CurrentRoll = GetActorRotation().Roll;
	// Roll의 절대값에 따라 MovementSpeed를 선형적으로 감소
	float RollFactor = FMath::Clamp(FMath::Abs(CurrentRoll) / 180.0f, 0.0f, 1.0f);

	// 에어 컨트롤 설정
	CurrentSpeed = bIsOnGround ? MovementSpeed : AirSpeed;

	// CurrnetSpeed를 Roll에 따라 조정 (Roll이 존재하면 1/3로 줄이기)
	if (!FMath::IsNearlyZero(CurrentRoll))
	{
		CurrentSpeed = InitialMovementSpeed * (1.0f - (2.0f / 3.0f) * RollFactor);
	}
	else
	{
		CurrentSpeed = InitialMovementSpeed; // Roll이 없을 때 기본 Speed 복구
	}

	// 공통 동작
	if (!FMath::IsNearlyZero(MoveInput.X))
	{
		MoveForward(MoveInput.X);
	}
	if (!FMath::IsNearlyZero(MoveInput.Y))
	{
		MoveRight(MoveInput.Y);
	}

	// 공중 모드에서만 Z축 이동 처리
	if (bIsInAir && !FMath::IsNearlyZero(MoveInput.Z))
	{
		MoveUp(MoveInput.Z);
	}
}
void ABroom::Look(const FInputActionValue& value)
{
	if (!Controller) return;

	const FVector LookInput = value.Get<FVector>();
	
	//// 디버그
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
				bIsInAir = true; // 상태 전환
				UE_LOG(LogTemp, Warning, TEXT("IsInAir?: %s"), bIsInAir ? TEXT("true") : TEXT("false"));
			}
			else
			{
				if (IMC_BroomFly && IMC_BroomGround)
				{
					InputSubsystem->RemoveMappingContext(IMC_BroomFly);
					InputSubsystem->AddMappingContext(IMC_BroomGround, 1);
				}
				bIsInAir = false; // 상태 전환
				UE_LOG(LogTemp, Warning, TEXT("IsInAir?: %s"), bIsInAir ? TEXT("true") : TEXT("false"));

				FallSpeed = 0.0f; // 이 때 낙하 속도를 초기화 해야 한다!
			}
		}
	}
	
}




