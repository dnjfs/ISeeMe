// Fill out your copyright notice in the Description page of Project Settings.


#include "ISMLobbyGameMode.h"
#include "ISMLobbyController.h"

void AISMLobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	PlayerNums = GetNumPlayers();

	AISMLobbyController* PC = Cast<AISMLobbyController>(GetWorld()->GetFirstPlayerController());
	if (PC == nullptr)
		return;
	
	//PC->InitUI();

	if (PlayerNums == 2)
	{
		PC->CallSelectChapterUI();
	}
}

void AISMLobbyGameMode::SelectCharacter()
{
	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		AISMLobbyController* PC = Cast<AISMLobbyController>(Iterator->Get());
		if (PC == nullptr)
			continue;

		PC->CallSelectCharacterUI();
	}
}
