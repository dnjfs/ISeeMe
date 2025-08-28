// Fill out your copyright notice in the Description page of Project Settings.

#include "ISMGameInstance.h"
#include "ISeeMe/ISMSaveGame.h"
#include "Kismet/GameplayStatics.h"


void UISMGameInstance::SaveGame()
{
	if (UISMSaveGame* SaveGameInstance = Cast<UISMSaveGame>(UGameplayStatics::CreateSaveGameObject(UISMSaveGame::StaticClass())))
	{
		SaveGameInstance->CurrChapterNo = this->CurrChapterNo;
		SaveGameInstance->MaxChapterNo = this->MaxChapterNo;
		SaveGameInstance->CheckPointID = this->SavedCheckPointID;
		SaveGameInstance->AcquiredRecollectionIDs = this->AcquiredRecollectionIDs;
		SaveGameInstance->bIsFirstLaunch = this->bIsFirstLaunch;

		UGameplayStatics::AsyncSaveGameToSlot(SaveGameInstance, TEXT("SaveSlot"), 0);
	}
}


void UISMGameInstance::LoadGame(UISMSaveGame* LoadedGame)
{
	CurrChapterNo = LoadedGame->CurrChapterNo;
	MaxChapterNo = LoadedGame->MaxChapterNo;
	SavedCheckPointID = LoadedGame->CheckPointID;
	AcquiredRecollectionIDs = LoadedGame->AcquiredRecollectionIDs;
	bIsFirstLaunch = LoadedGame->bIsFirstLaunch;
}

void UISMGameInstance::SetSelectedPawnClass(TSubclassOf<ACharacter> ServerCharacter, TSubclassOf<ACharacter> ClientCharacter)
{
	UWorld* World = GetWorld();
	if (!IsValid(World))
	{
		return;
	}

	// 서버에서만 실행
	if (World->GetNetMode() < ENetMode::NM_Client)
	{
		SelectedPawnClass = ServerCharacter;
		ClientPawnClass = ClientCharacter;
	}
}