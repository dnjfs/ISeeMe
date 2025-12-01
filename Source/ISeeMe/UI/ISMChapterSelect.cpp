// Fill out your copyright notice in the Description page of Project Settings.


#include "ISMChapterSelect.h"
#include "ISeeMe/ISMGameInstance.h"
#include "ISeeMe/ISMSaveGame.h"
#include "Kismet/GameplayStatics.h"
#include "ISeeMe/ISMLobbyController.h"
#include "UMG.h"
#include "ISMChapterList.h"
#include <ISeeMe/ISMLobbyGameMode.h>
#include <ISeeMe/ISMLobbyGameState.h>

void UISMChapterSelect::NativeConstruct()
{
	Super::NativeConstruct();

	if (UISMGameInstance* GI = GetGameInstance<UISMGameInstance>())
	{
		// セーブファイルが存在
		if (UISMSaveGame* LoadedGame = Cast<UISMSaveGame>(UGameplayStatics::LoadGameFromSlot("SaveSlot", 0)))
		{
			ContinueButton->SetIsEnabled(true);
			GI->LoadGame(LoadedGame);
		}
		else // セーブファイルが存在しない
		{
			ContinueButton->SetIsEnabled(false);
			GI->CurrChapterNo = 0;
			GI->MaxChapterNo = 0;
			GI->SavedCheckPointID = FName("None");
		}

		WBP_ISMChapterList->EnableChapterSelectButton(GI->MaxChapterNo);

		NewGameButton->OnClicked.AddDynamic(this, &UISMChapterSelect::NewGame);
		ContinueButton->OnClicked.AddDynamic(this, &UISMChapterSelect::Continue);
		BackButton->OnClicked.AddDynamic(this, &UISMChapterSelect::BackLobbyUI);
	}
}

void UISMChapterSelect::NewGame()
{
	if (UISMGameInstance* GI = GetGameInstance<UISMGameInstance>())
	{
		// 現在のチャプターとチェックポイント情報を初期化して保存
		GI->CurrChapterNo = 0;
		GI->SavedCheckPointID = FName("None");

		GI->SaveGame();
		GI->bTutorial = false;

		FString ChapterName = FString::Printf(TEXT("Chapter%d"), GI->CurrChapterNo);
		if (AISMLobbyGameMode* GM = Cast<AISMLobbyGameMode>(GetWorld()->GetAuthGameMode()))
		{
			GM->SelectCharacterUI();
		}
	}
}

void UISMChapterSelect::Continue()
{
	if (UISMGameInstance* GI = GetGameInstance<UISMGameInstance>())
	{
		FString ChapterName = FString::Printf(TEXT("Chapter%d"), GI->CurrChapterNo);
		GI->bTutorial = true;
		if (AISMLobbyGameMode* GM = Cast<AISMLobbyGameMode>(GetWorld()->GetAuthGameMode()))
		{
			GM->SelectCharacterUI();
		}
	}
}

void UISMChapterSelect::BackLobbyUI()
{
	if (AISMLobbyGameState* GS = Cast<AISMLobbyGameState>(GetWorld()->GetGameState()))
	{
		GS->InitSelect();
		if (AISMLobbyController* Controller = Cast<AISMLobbyController>(GetOwningPlayer()))
		{
			Controller->CallBackUI("Lobby");
		}
	}
}

