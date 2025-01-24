// Fill out your copyright notice in the Description page of Project Settings.
#include "BroomController.h"
#include "EnhancedInputSubsystems.h"

ABroomController::ABroomController()
	: InputMappingContext(nullptr),
	  MoveAction(nullptr),
	  LookAction(nullptr),
	  SwitchAction(nullptr) { }

void ABroomController::BeginPlay()
{
	Super::BeginPlay();

	if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			if (InputMappingContext)
			{
				Subsystem->AddMappingContext(InputMappingContext, 0);
			}
		}
	}
}