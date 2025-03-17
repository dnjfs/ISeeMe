// Fill out your copyright notice in the Description page of Project Settings.


#include "ISMLobbyGameMode.h"
#include "Blueprint/UserWidget.h"
#include "UI/ISMLobbyMenu.h"
#include <Components/WidgetSwitcher.h>
#include "UI/ISMCharacterSelect.h"
#include "ISMLobbyGameState.h"

void AISMLobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	int PlayerNums = GetNumPlayers();

	AISMLobbyController* PC = Cast<AISMLobbyController>(GetWorld()->GetFirstPlayerController());
	if (PC == nullptr)
		return;

	if (PlayerNums == 2)
	{
		PC->SelectChapterUI();
	}

	int index = 0;
	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		AISMLobbyController* Controller = Cast<AISMLobbyController>(Iterator->Get());
		if (Controller == nullptr)
			continue;

		PCs[index] = Controller;
		index++;
	}
}

void AISMLobbyGameMode::SelectCharacterUI()
{
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
	if (AISMLobbyGameState* GS = Cast<AISMLobbyGameState>(GetWorld()->GetGameState()))
	{
		int OtherIndex = (index + 1) % 2;

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
