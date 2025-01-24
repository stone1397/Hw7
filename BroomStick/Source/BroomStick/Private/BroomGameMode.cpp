// Fill out your copyright notice in the Description page of Project Settings.


#include "BroomGameMode.h"
#include "Broom.h"
#include "BroomController.h"

ABroomGameMode::ABroomGameMode()
{
	DefaultPawnClass = ABroom::StaticClass();
	PlayerControllerClass = ABroomController::StaticClass();
}
