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
	if (CurrentPlayers != 2)
		return;

	TArray<AISMPlayerController*> PCs;
	TArray<ACharacter*> Characters;

	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		AISMPlayerController* PC = Cast<AISMPlayerController>(Iterator->Get());
		if (PC == nullptr)
			continue;

		PCs.Add(PC);

		ACharacter* Character = PC->GetCharacter();
		if (Character == nullptr)
			continue;

		Characters.Add(Character);
	}


	// 2인 플레이인지 확인
	check(Characters.Num() == 2);

	PCs[0]->SetViewTarget(Characters[1]);
	PCs[0]->SetOtherCharacter(Characters[1]);

	PCs[1]->SetViewTarget(Characters[0]);
	PCs[1]->SetOtherCharacter(Characters[0]);
}