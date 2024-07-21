// Copyright Epic Games, Inc. All Rights Reserved.

#include "ISeeMeGameMode.h"
#include "ISeeMeCharacter.h"
#include "UObject/ConstructorHelpers.h"

AISeeMeGameMode::AISeeMeGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
