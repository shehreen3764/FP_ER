// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "FP_ERGameMode.h"
#include "FP_ERHUD.h"
#include "FP_ERCharacter.h"
#include "UObject/ConstructorHelpers.h"

AFP_ERGameMode::AFP_ERGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = AFP_ERHUD::StaticClass();
}
