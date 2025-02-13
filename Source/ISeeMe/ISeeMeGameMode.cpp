// Copyright Epic Games, Inc. All Rights Reserved.

#include "ISeeMeGameMode.h"
#include "ISeeMeCharacter.h"
#include "UObject/ConstructorHelpers.h"

#include "ISMPlayerController.h"
#include "ISMCharacterState.h"
#include "ISMGameInstance.h"
#include "ISMLobbyController.h"
#include <Kismet/GameplayStatics.h>
#include "UI/ISMHUD.h"
#include "ISeeMe/UI/ISMOverlay.h"

AISeeMeGameMode::AISeeMeGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	SelectedPawnClasses.Add(nullptr);
	SelectedPawnClasses.Add(nullptr);
}


void AISeeMeGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	if (AISMCharacterState* State = NewPlayer->GetPlayerState<AISMCharacterState>())
	{
		int32 CurrentPlayerCount = GetNumPlayers();
		State->CustomPlayerID = CurrentPlayerCount;
	}
}

void AISeeMeGameMode::SwapCamera()
{
	LOG_SCREEN("Swap");
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
			PCs[i]->SetViewTarget(Characters[i]);
			PCs[i]->SetOtherCharacter(Characters[i]);
			PCs[i]->CurrentAspect();

			if (AISeeMeCharacter* ISeeMeCharacter = PCs[i]->GetPawn<AISeeMeCharacter>())
			{
				ISeeMeCharacter->IsCameraRestored = true;
				/*if (PlayerNum == 2)
				{
					ISeeMeCharacter->EnableAudio();
					Characters[(i + 1 < PlayerNum) ? i + 1 : 0]->DisableAudio();
				}*/
			}
		}
	}
	else
	{
		for (int i = 0; i < PlayerNum; i++)
		{
			PCs[i]->SetViewTarget(Characters[(i + 1 < PlayerNum) ? i + 1 : 0]);
			PCs[i]->SetOtherCharacter(Characters[(i + 1 < PlayerNum) ? i + 1 : 0]);
			PCs[i]->CurrentAspect();
			
			if (AISeeMeCharacter* ISeeMeCharacter = PCs[i]->GetPawn<AISeeMeCharacter>())
			{
				ISeeMeCharacter->IsCameraRestored = false;
				/*if (PlayerNum == 2)
				{
					ISeeMeCharacter->DisableAudio();
					Characters[(i + 1 < PlayerNum) ? i + 1 : 0]->EnableAudio();
				}*/
			}
		}
	}
	bSwapCamera = !bSwapCamera;
}

void AISeeMeGameMode::ChangePawn()
{
	TArray<AISeeMeCharacter*> LocalCharacters;
	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		AISMPlayerController* PC = Cast<AISMPlayerController>(Iterator->Get());
		if (PC == nullptr)
			continue;

		ACharacter* BaseCharacter = PC->GetCharacter();
		if (BaseCharacter && BaseCharacter->IsA(AISeeMeCharacter::StaticClass()))
		{
			AISeeMeCharacter* LocalCharacter = Cast<AISeeMeCharacter>(BaseCharacter);
			LocalCharacters.Add(LocalCharacter);
		}
	}

	if (LocalCharacters.Num() != SelectedPawnClasses.Num())
	{
		UE_LOG(LogTemp, Error, TEXT("Mismatch between Characters (%d) and SelectedPawnClasses (%d) count!"),
			LocalCharacters.Num(), SelectedPawnClasses.Num());
		return;
	}

	for (int i = 0; i < LocalCharacters.Num(); i++)
	{
		AISeeMeCharacter* MyCharacter = LocalCharacters[i];
		if (SelectedPawnClasses[i])
		{
			APawn* ExistingPawn = MyCharacter->GetController()->GetPawn();
			AISMPlayerController* Controller = Cast<AISMPlayerController>(MyCharacter->GetController());
			if (ExistingPawn)
			{
				UE_LOG(LogTemp, Warning, TEXT("Destroying existing pawn for player %d: %s"), i, *ExistingPawn->GetName());
				ExistingPawn->Destroy();
			}

			FVector MySpawnLocation = MyCharacter->GetActorLocation();
			FRotator MySpawnRotation = MyCharacter->GetActorRotation();
			if (!MySpawnLocation.ContainsNaN() && !MySpawnRotation.ContainsNaN())
			{
				APawn* NewPawn = GetWorld()->SpawnActor<APawn>(SelectedPawnClasses[i], MySpawnLocation, MySpawnRotation);

				if (NewPawn)
				{
					Controller->Possess(NewPawn);
					Controller->MulticastUpdateController();
				}
			}
		}
	}
}
