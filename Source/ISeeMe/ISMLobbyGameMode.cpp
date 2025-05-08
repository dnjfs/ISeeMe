// Fill out your copyright notice in the Description page of Project Settings.


#include "ISMLobbyGameMode.h"
#include "Blueprint/UserWidget.h"
#include "UI/ISMLobbyMenu.h"
#include <Components/WidgetSwitcher.h>
#include "UI/ISMCharacterSelect.h"
#include "ISMLobbyGameState.h"
#include "ISMGameInstance.h"

void AISMLobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	int PlayerNums = GetNumPlayers();

	AISMLobbyController* PC = Cast<AISMLobbyController>(GetWorld()->GetFirstPlayerController());
	if (PC == nullptr)
		return;

	if (PlayerNums == 2)
	{
		PC->ControllerChangeLobbyUI(2);
	}

	int index = 0;
	// Init Player Controllers
	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		AISMLobbyController* Controller = Cast<AISMLobbyController>(Iterator->Get());
		if (Controller == nullptr)
			continue;

		if (index >= PCs.Num())
			break;

		PCs[index] = Controller;
		index++;
	}
}

void AISMLobbyGameMode::SelectCharacterUI()
{
	// Show Select Character UI
	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		AISMLobbyController* PC = Cast<AISMLobbyController>(Iterator->Get());
		if (PC == nullptr)
			continue;

		PC->CallSelectCharacterUI();
	} 
}

void AISMLobbyGameMode::ChangeCharacterButton(FString CharacterSelect, int index)
{
	int OtherIndex = (index + 1) % 2;

	// Init Select Character
	if (CharacterSelect == "None")
	{
		if (AISMLobbyGameState* GS = Cast<AISMLobbyGameState>(GetWorld()->GetGameState()))
		{
			GS->InitSelect();
		}

		PCs[index]->MulticastChangeCharacterButton(CharacterSelect, FLinearColor(1, 1, 1, 0.1f));
		PCs[OtherIndex]->MulticastChangeCharacterButton(CharacterSelect, FLinearColor(1, 1, 1, 0.1f));
		return;
	} 

	if (AISMLobbyGameState* GS = Cast<AISMLobbyGameState>(GetWorld()->GetGameState()))
	{
		// Determine character selection
		bool& bSelectedCurrent = (CharacterSelect == "Hojin") ? GS->bSelectedHojin[index] : GS->bSelectedMimi[index];
		bool& bSelectedOther = (CharacterSelect == "Hojin") ? GS->bSelectedHojin[OtherIndex] : GS->bSelectedMimi[OtherIndex];
		bool& bDeselectedCurrent = (CharacterSelect == "Hojin") ? GS->bSelectedMimi[index] : GS->bSelectedHojin[index];
		bool& bDeselectedOther = (CharacterSelect == "Hojin") ? GS->bSelectedMimi[OtherIndex] : GS->bSelectedHojin[OtherIndex];

		// Set current character selection
		bSelectedCurrent = true;

		// Handle color changes based on selection state
		if (bSelectedOther)
		{
			PCs[index]->MulticastChangeCharacterButton(CharacterSelect, FLinearColor::Red);
			PCs[OtherIndex]->MulticastChangeCharacterButton(CharacterSelect, FLinearColor::Red);
		}
		else
		{
			PCs[index]->MulticastChangeCharacterButton(CharacterSelect, FLinearColor::Blue);
			PCs[OtherIndex]->MulticastChangeCharacterButton(CharacterSelect, FLinearColor::Green);
		}

		// Reset the opposite selection
		if (bDeselectedCurrent)
		{
			bDeselectedCurrent = false;
		}

		// Handle deselected color states
		if (bDeselectedOther && !bSelectedOther)
		{
			PCs[index]->MulticastChangeCharacterButton(CharacterSelect == "Hojin" ? "Mimi" : "Hojin", FLinearColor::Green);
			PCs[OtherIndex]->MulticastChangeCharacterButton(CharacterSelect == "Hojin" ? "Mimi" : "Hojin", FLinearColor::Blue);
		}
		else
		{
			PCs[index]->MulticastChangeCharacterButton(CharacterSelect == "Hojin" ? "Mimi" : "Hojin", FLinearColor(1, 1, 1, 0.1f));
			PCs[OtherIndex]->MulticastChangeCharacterButton(CharacterSelect == "Hojin" ? "Mimi" : "Hojin", FLinearColor(1, 1, 1, 0.1f));
		}
	}
}

void AISMLobbyGameMode::BackChapterUI()
{
	PCs[0]->MulticastControllerChangeUI(2);
	PCs[1]->MulticastControllerChangeUI(3);
}

void AISMLobbyGameMode::CountReadTutorial()
{
	ReadTutorial++;

	if (ReadTutorial == 2)
	{
		if (UISMGameInstance* GI = GetGameInstance<UISMGameInstance>())
		{
			LoadingNextLevel();
		}
	}
}

// Show Tutorial Widget
void AISMLobbyGameMode::ChangeTutorial()
{
	PCs[0]->CallTutorial();
	PCs[1]->CallTutorial();
}

void AISMLobbyGameMode::LoadingNextLevel()
{
	if (UISMGameInstance* GI = GetGameInstance<UISMGameInstance>())
	{
		FString ChapterName = FString::Printf(TEXT("Chapter%d"), GI->CurrChapterNo);
		UWorld* World = GetWorld();
		PCs[0]->MulticastControllerChangeUI(6);
		PCs[1]->MulticastControllerChangeUI(6);
		World->ServerTravel("/Game/ISeeMe/Maps/" + ChapterName + "?listen", true);
	}
}
