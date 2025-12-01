// Copyright Epic Games, Inc. All Rights Reserved.

#include "ISeeMeGameMode.h"
#include "ISeeMeCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "ISMTutorialController.h"
#include "ISMPlayerController.h"
#include "ISMCharacterState.h"
#include "ISMGameInstance.h"
#include "ISMLobbyController.h"
#include <Kismet/GameplayStatics.h>
#include "UI/ISMHUD.h"
#include "ISeeMe/UI/ISMOverlay.h"
#include "GameFramework/CharacterMovementComponent.h"
#include <EnhancedInputSubsystems.h>
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "ISMGameState.h"
#include "GameFramework/PlayerState.h"

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

	UISMGameInstance* GI = Cast<UISMGameInstance>(GetGameInstance());

	if (GetNumPlayers() == 2)
	{
		for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
		{
			AISMPlayerController* PC = Cast<AISMPlayerController>(Iterator->Get());

			// Enable Input
			if (PC == nullptr)
				continue;

			ACharacter* BaseCharacter = PC->GetCharacter();
			if (BaseCharacter)
			{
				BaseCharacter->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
			}
		}

		if (!bFirst)
		{
			for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
			{
				AISMPlayerController* PC = Cast<AISMPlayerController>(Iterator->Get());

				// Go Check Point
				if (GI->SavedCheckPointID != FName("None"))
				{
					TArray<AActor*> CheckPoints;
					UGameplayStatics::GetAllActorsOfClass(GetWorld(), AISMCheckPoint::StaticClass(), CheckPoints);
					for (AActor* ACheckPoint : CheckPoints)
					{
						if (GI->SavedCheckPointID == ACheckPoint->GetFName()) // チェックポイントを見つけた
						{
							AISMCheckPoint* ISMCheckPoint = Cast<AISMCheckPoint>(ACheckPoint);

							if (AISMGameState* GS = Cast<AISMGameState>(UGameplayStatics::GetGameState(this)))
							{
								GS->SaveSwapViewItem = GS->SwapViewItem;
								GS->UsedSwapViewItems.Empty();

								ISMCheckPoint->MulticastChangeMaterial(2);
								ISMCheckPoint->InitCheckPoint();

								PC->DeadCharacter();
							}
							break;
						}
						else
						{
							continue;
						}
					}
				}
			}
		}
		else
		{
			bFirst = false;
		}
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
		if (!IsValid(PC))
			continue;

		AISeeMeCharacter* Character = Cast<AISeeMeCharacter>(PC->GetCharacter());
		if (!IsValid(Character))
			continue;

		PCs.Add(PC);
		Characters.Add(Character);
	}

	// カメラをスワップ
	int PlayerNum = PCs.Num();
	for (int i = 0; i < PlayerNum; i++)
	{
		int TargetIndex = (i + 1 < PlayerNum) ? i + 1 : 0;
		AISeeMeCharacter* Character = Characters[TargetIndex];
		check(Character); // nullptr ではないと仮定

		PCs[i]->SetViewTarget(Character);
		PCs[i]->SetOtherCharacter(Character);
		PCs[i]->CurrentAspect();

		Character->MulticastSetCameraRestore(false);
	}

	bSwapCamera = true;
}

void AISeeMeGameMode::RestoreCamera()
{
	int32 CurrentPlayers = GetNumPlayers();

	TArray<AISMPlayerController*> PCs;
	TArray<AISeeMeCharacter*> Characters;

	// カメラ復旧
	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		AISMPlayerController* PC = Cast<AISMPlayerController>(Iterator->Get());
		if (PC == nullptr)
			continue;

		AISeeMeCharacter* Character = Cast<AISeeMeCharacter>(PC->GetCharacter());
		if (Character == nullptr)
			continue;

		PC->SetViewTarget(Character);
		PC->SetOtherCharacter(Character);
		PC->CurrentAspect();

		Character->MulticastSetCameraRestore(true);
	}

	bSwapCamera = false;
}

bool AISeeMeGameMode::ChangePawn(APlayerController* Controller, TSubclassOf<APawn> SelectedPawnClass)
{
	if (!IsValid(Controller))
	{
		return false;
	}

	if (!SelectedPawnClass)
	{
		return false;
	}

	APawn* ExistingPawn = Controller->GetPawn();
	if (!IsValid(ExistingPawn))
	{
		return false;
	}

	FVector MySpawnLocation = ExistingPawn->GetActorLocation();
	FRotator MySpawnRotation = ExistingPawn->GetActorRotation();

	APawn* NewPawn = GetWorld()->SpawnActor<APawn>(SelectedPawnClass, MySpawnLocation, MySpawnRotation);
	if (!NewPawn)
	{
		return false;
	}

	ExistingPawn->Destroy();
	Controller->Possess(NewPawn);

	return true;
}

void AISeeMeGameMode::CountReadEnding()
{
	ReadEnding++;

	if (ReadEnding == 2)
	{
		// Logic After All Enter 

		// Move Lobby Logic
		for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
		{
			APlayerController* PCs = It->Get();
			if (PCs && !PCs->IsLocalController())
			{
				AISMPlayerController* PC = Cast<AISMPlayerController>(PCs);
				if (PC)
				{
					PC->ClientGoToLevel();
				}
			}
		}

		IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
		if (OnlineSub)
		{
			IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
			if (Sessions.IsValid() && Sessions->GetNamedSession(NAME_GameSession))
			{
				OnDestroySessionCompleteDelegate.BindUObject(this, &AISeeMeGameMode::OnDestroySessionComplete);
				Sessions->AddOnDestroySessionCompleteDelegate_Handle(OnDestroySessionCompleteDelegate);
				Sessions->DestroySession(NAME_GameSession);
			}
			else
			{
				OnDestroySessionComplete(NAME_None, true);
			}
		}
	}
}

void AISeeMeGameMode::OnDestroySessionComplete(FName SessionName, bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		UGameplayStatics::OpenLevel(GetWorld(), FName("LoadingMap"), true);
	}
}

void AISeeMeGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);

	// Disable Input
	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		AISMPlayerController* PC = Cast<AISMPlayerController>(Iterator->Get());
		if (PC == nullptr)
			continue;

		if (PC->IsLocalController())
		{
			ACharacter* BaseCharacter = PC->GetCharacter();
			if (BaseCharacter)
			{
				BaseCharacter->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
			}
		}
	}
}
