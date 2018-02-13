// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "GeodroidGameMode.h"
#include "GeodroidHUD.h"
#include "GeodroidCharacter.h"
#include "UObject/ConstructorHelpers.h"

AGeodroidGameMode::AGeodroidGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = AGeodroidHUD::StaticClass();
}
