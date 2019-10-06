// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "RPGFPSGameMode.h"
#include "RPGFPSHUD.h"
#include "RPGFPSCharacter.h"
#include "UObject/ConstructorHelpers.h"

ARPGFPSGameMode::ARPGFPSGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = ARPGFPSHUD::StaticClass();
}
