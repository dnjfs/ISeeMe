// Fill out your copyright notice in the Description page of Project Settings.

#include "ISMGameInstance.h"
#include "ISeeMe/ISMSaveGame.h"
#include "Kismet/GameplayStatics.h"

void UISMGameInstance::LoadGame(UISMSaveGame* LoadedGame)
{
	CurrChapterNo = LoadedGame->CurrChapterNo;
	MaxChapterNo = LoadedGame->MaxChapterNo;
	SavedCheckPointID = LoadedGame->CheckPointID;
}

void UISMGameInstance::SaveGame()
{
	if (UISMSaveGame* SaveGameInstance = Cast<UISMSaveGame>(UGameplayStatics::CreateSaveGameObject(UISMSaveGame::StaticClass())))
	{
		SaveGameInstance->CurrChapterNo = this->CurrChapterNo;
		SaveGameInstance->MaxChapterNo = this->MaxChapterNo;
		SaveGameInstance->CheckPointID = this->SavedCheckPointID;

		UGameplayStatics::AsyncSaveGameToSlot(SaveGameInstance, TEXT("SaveSlot"), 0);
	}
}
