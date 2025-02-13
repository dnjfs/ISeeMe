// Fill out your copyright notice in the Description page of Project Settings.


#include "ISMLobby.h"
#include "ISeeMe/ISMGameInstance.h"
#include "ISeeMe/ISMSaveGame.h"
#include "Kismet/GameplayStatics.h"
#include "ISeeMe/ISMLobbyController.h"
#include "UMG.h"

void UISMLobby::NativeConstruct()
{
	Super::NativeConstruct();

	if (UISMGameInstance* GI = GetGameInstance<UISMGameInstance>())
	{
		// 세이브 파일 존재
		if (UISMSaveGame* LoadedGame = Cast<UISMSaveGame>(UGameplayStatics::LoadGameFromSlot("SaveSlot", 0)))
		{
			ContinueButton->SetIsEnabled(true);
			GI->CurrChapterNo = LoadedGame->CurrChapterNo;
			GI->SavedCheckPointID = LoadedGame->CheckPointID;
		}
		else // 세이브 파일 없음
		{
			ContinueButton->SetIsEnabled(false);
			GI->CurrChapterNo = 1;
			GI->SavedCheckPointID = FName("None");
		}
	}

	NewGameButton->OnClicked.AddDynamic(this, &UISMLobby::NewGame);
	ContinueButton->OnClicked.AddDynamic(this, &UISMLobby::Continue);
}

void UISMLobby::NewGame()
{
	if (UISMGameInstance* GI = GetGameInstance<UISMGameInstance>())
	{
		// 챕터, 체크포인트 정보 초기화 후 저장
		GI->CurrChapterNo = 1;
		GI->SavedCheckPointID = FName("None");

		if (UISMSaveGame* SaveGameInstance = Cast<UISMSaveGame>(UGameplayStatics::CreateSaveGameObject(UISMSaveGame::StaticClass())))
		{
			SaveGameInstance->CurrChapterNo = GI->CurrChapterNo;
			SaveGameInstance->CheckPointID = GI->SavedCheckPointID;

			UGameplayStatics::AsyncSaveGameToSlot(SaveGameInstance, TEXT("SaveSlot"), 0);
		}

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
