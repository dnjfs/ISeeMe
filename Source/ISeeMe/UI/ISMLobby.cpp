// Fill out your copyright notice in the Description page of Project Settings.


#include "ISMLobby.h"
#include "ISeeMe/ISMGameInstance.h"
#include "ISeeMe/ISMSaveGame.h"
#include "Kismet/GameplayStatics.h"
#include "ISeeMe/ISMLobbyController.h"
#include "UMG.h"
#include "ISMChapterSelect.h"

void UISMLobby::NativeConstruct()
{
	Super::NativeConstruct();

	if (UISMGameInstance* GI = GetGameInstance<UISMGameInstance>())
	{
		// 세이브 파일 존재
		if (UISMSaveGame* LoadedGame = Cast<UISMSaveGame>(UGameplayStatics::LoadGameFromSlot("SaveSlot", 0)))
		{
			ContinueButton->SetIsEnabled(true);
			GI->LoadGame(LoadedGame);
		}
		else // 세이브 파일 없음
		{
			ContinueButton->SetIsEnabled(false);
			GI->CurrChapterNo = 1;
			GI->MaxChapterNo = 1;
			GI->SavedCheckPointID = FName("None");
		}

		WBP_ISMChapterSelect->EnableChapterSelectButton(GI->MaxChapterNo);
	}

	NewGameButton->OnClicked.AddDynamic(this, &UISMLobby::NewGame);
	ContinueButton->OnClicked.AddDynamic(this, &UISMLobby::Continue);
}

void UISMLobby::NewGame()
{
	if (UISMGameInstance* GI = GetGameInstance<UISMGameInstance>())
	{
		// 현재 챕터, 체크포인트 정보 초기화 후 저장
		GI->CurrChapterNo = 1;
		GI->SavedCheckPointID = FName("None");

		GI->SaveGame();

		if (AISMLobbyController* LobbyController = GetWorld()->GetFirstPlayerController<AISMLobbyController>())
		{
			FString ChapterName = FString::Printf(TEXT("Chapter%d"), GI->CurrChapterNo);
			LobbyController->CreateSession(FName(*ChapterName));
		}
	}
}

void UISMLobby::Continue()
{
	if (UISMGameInstance* GI = GetGameInstance<UISMGameInstance>())
	{
		if (AISMLobbyController* LobbyController = GetWorld()->GetFirstPlayerController<AISMLobbyController>())
		{
			FString ChapterName = FString::Printf(TEXT("Chapter%d"), GI->CurrChapterNo);
			LobbyController->CreateSession(FName(*ChapterName));
		}
	}
}
