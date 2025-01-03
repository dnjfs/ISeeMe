// Copyright Epic Games, Inc. All Rights Reserved.

#include "ISeeMeGameMode.h"
#include "ISeeMeCharacter.h"
#include "UObject/ConstructorHelpers.h"

#include "ISMPlayerController.h"
#include "ISMCharacterState.h"

AISeeMeGameMode::AISeeMeGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}

void AISeeMeGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	if (AISMCharacterState* State = NewPlayer->GetPlayerState<AISMCharacterState>())
	{
		int32 CurrentPlayerCount = GetNumPlayers();
		State->CustomPlayerID = CurrentPlayerCount; // First Player = 1, Second Player = 2
	}
}

void AISeeMeGameMode::SwapCamera()
{
	int32 CurrentPlayers = GetNumPlayers();

	TArray<AISMPlayerController*> PCs;
	TArray<AISeeMeCharacter*> Characters;

	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		AISMPlayerController* PC = Cast<AISMPlayerController>(Iterator->Get());
		if (PC == nullptr)
			continue;

		ACharacter* BaseCharacter = PC->GetCharacter();

		if (BaseCharacter == nullptr)
			continue;

		if (BaseCharacter && BaseCharacter->IsA(AISeeMeCharacter::StaticClass()))
		{
			AISeeMeCharacter* Character = Cast<AISeeMeCharacter>(BaseCharacter);
			PCs.Add(PC);
			Characters.Add(Character);
		}
	}

	// 카메라 스왑
	int PlayerNum = PCs.Num();
	if (bSwapCamera)
	{
		for (int i = 0; i < PlayerNum; i++)
		{
			PCs[i]->RecoverAspect();
			PCs[i]->SetViewTarget(Characters[i]);
			PCs[i]->SetOtherCharacter(Characters[i]);
			PCs[i]->CurrentAspect();

			if (AISeeMeCharacter* ISeeMeCharacter = PCs[i]->GetPawn<AISeeMeCharacter>())
			{
				ISeeMeCharacter->IsCameraRestored = true;
			}
		}
	}
	else
	{
		for (int i = 0; i < PlayerNum; i++)
		{
			PCs[i]->RecoverAspect();
			PCs[i]->SetViewTarget(Characters[(i + 1 < PlayerNum) ? i + 1 : 0]);
			PCs[i]->SetOtherCharacter(Characters[(i + 1 < PlayerNum) ? i + 1 : 0]);
			PCs[i]->CurrentAspect();
			
			if (AISeeMeCharacter* ISeeMeCharacter = PCs[i]->GetPawn<AISeeMeCharacter>())
			{
				ISeeMeCharacter->IsCameraRestored = false;
			}
		}
	}
	bSwapCamera = !bSwapCamera;
}