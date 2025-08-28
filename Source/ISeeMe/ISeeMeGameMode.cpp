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

	SelectedPawnClasses.Add(nullptr);
	SelectedPawnClasses.Add(nullptr);
}


void AISeeMeGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	UISMGameInstance* GI = Cast<UISMGameInstance>(GetGameInstance());
	AISMPlayerController* PC = Cast<AISMPlayerController>(NewPlayer);

	if (GetNumPlayers() == 2)
	{
		if (!bFirst)
		{
			for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
			{
				// Enable Input
				if (PC == nullptr)
					continue;

				ACharacter* BaseCharacter = PC->GetCharacter();
				if (BaseCharacter)
				{
					//BaseCharacter->EnableInput(PC);
					BaseCharacter->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
				}

				// Go Check Point
				if (GI->SavedCheckPointID != FName("None"))
				{
					TArray<AActor*> CheckPoints;
					UGameplayStatics::GetAllActorsOfClass(GetWorld(), AISMCheckPoint::StaticClass(), CheckPoints);
					for (AActor* ACheckPoint : CheckPoints)
					{
						if (GI->SavedCheckPointID == ACheckPoint->GetFName()) // 체크포인트 찾음
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
		if (PC == nullptr)
			continue;

		AISeeMeCharacter* Character = Cast<AISeeMeCharacter>(PC->GetCharacter());
		if (Character == nullptr)
			continue;

		PCs.Add(PC);
		Characters.Add(Character);
	}

	// 카메라 스왑
	int PlayerNum = PCs.Num();
	for (int i = 0; i < PlayerNum; i++)
	{
		int TargetIndex = (i+1 < PlayerNum) ? i+1 : 0;
		AISeeMeCharacter* Character = Characters[TargetIndex];
		check(Character); // nullptr이 아닐거라 가정

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

	// 카메라 복구
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

void AISeeMeGameMode::ChangePawn()
{
	UE_LOG(LogTemp, Warning, TEXT("Swap Pawn"));

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

	for (int i = 0; i < LocalCharacters.Num(); i++)
	{
		AISeeMeCharacter* MyCharacter = LocalCharacters[i];
		if (!IsValid(MyCharacter))
			continue;

		AISMPlayerController* Controller = Cast<AISMPlayerController>(MyCharacter->GetController());
		if (!Controller)
			continue;

		if (AISMCharacterState* State = Controller->GetPlayerState<AISMCharacterState>())
		{
			State->CustomPlayerID = i + 1;
		}

		if (!SelectedPawnClasses[i])
			continue;

		if (APawn* ExistingPawn = Controller->GetPawn(); IsValid(ExistingPawn))
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
			}
		}
	}
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

void AISeeMeGameMode::SwapPlayerControllers(APlayerController* OldPC, APlayerController* NewPC)
{
	Super::SwapPlayerControllers(OldPC, NewPC);

	/*if (NewPC)
	{
		if (AISMCharacterState* State = NewPC->GetPlayerState<AISMCharacterState>())
		{
			int32 CurrentPlayerCount = State->PlayerId;
			UE_LOG(LogTemp, Warning, TEXT("SwapPlayerControllers: Player count is %d"), CurrentPlayerCount);
			State->CustomPlayerID = CurrentPlayerCount;
		}
	}*/

	UE_LOG(LogTemp, Warning, TEXT("Swap Controllers %s -> %s"), *OldPC->GetName(), *NewPC->GetName());
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

		ACharacter* BaseCharacter = PC->GetCharacter();
		if (BaseCharacter)
		{
			BaseCharacter->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
		}
	}
}