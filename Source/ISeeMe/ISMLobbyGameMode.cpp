// Fill out your copyright notice in the Description page of Project Settings.


#include "ISMLobbyGameMode.h"
#include "ISMLobbyController.h"
#include "UI/ISMStartController.h"

void AISMLobbyGameMode::ChangeSelectCharacter()
{
	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		AISMStartController* PC = Cast<AISMStartController>(Iterator->Get());
		if (PC == nullptr)
			continue;

		PC->CallSelectCharacterUI();
	}
}

void AISMLobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
    Super::PostLogin(NewPlayer);

	if (GetNumPlayers() == 2)
	{
		for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
		{
			AISMStartController* PC = Cast<AISMStartController>(Iterator->Get());

			if (PC == nullptr)
				continue;

			//PC->InitUI();
		}
	}
}