// Copyright Epic Games, Inc. All Rights Reserved.

#include "ISeeMeGameMode.h"
#include "ISeeMeCharacter.h"
#include "UObject/ConstructorHelpers.h"

#include "ISMPlayerController.h"

AISeeMeGameMode::AISeeMeGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}

void AISeeMeGameMode::SwapCamera()
{
	int32 CurrentPlayers = GetNumPlayers();

	TArray<AISMPlayerController*> PCs;
	TArray<ACharacter*> Characters;

	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		AISMPlayerController* PC = Cast<AISMPlayerController>(Iterator->Get());
		if (PC == nullptr)
			continue;

		ACharacter* Character = PC->GetCharacter();
		if (Character == nullptr)
			continue;

		PCs.Add(PC);
		Characters.Add(Character);
	}

	// 카메라 스왑
	int PlayerNum = PCs.Num();
	for (int i = 0; i < PlayerNum; i++)
	{
		PCs[i]->SetViewTarget(Characters[(i+1 < PlayerNum) ? i+1 : 0]);
		PCs[i]->SetOtherCharacter(Characters[(i+1 < PlayerNum) ? i+1 : 0]);
	}
}