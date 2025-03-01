// Fill out your copyright notice in the Description page of Project Settings.


#include "ISMChapterSelect.h"
#include "ISeeMe/ISMGameInstance.h"
#include "ISeeMe/ISMSaveGame.h"
#include "Kismet/GameplayStatics.h"
#include "UMG.h"
#include "ISeeMe/ISMLobbyController.h"

void UISMChapterSelect::EnableChapterSelectButton(int MaxChapterNo)
{
	for (int i = 0; i < MaxChapterNo; i++)
	{
		if (UWidget* ChapterButton = ChapterListBox->GetChildAt(i))
		{
			ChapterButton->SetIsEnabled(true);
		}
	}
}

void UISMChapterSelect::SelectChapter(int ChapterNo)
{
	if (UISMGameInstance* GI = GetGameInstance<UISMGameInstance>())
	{
		// 현재 챕터, 체크포인트 정보 초기화 후 저장
		GI->CurrChapterNo = ChapterNo;
		GI->SavedCheckPointID = FName("None");

		GI->SaveGame();

		if (AISMLobbyController* LobbyController = GetWorld()->GetFirstPlayerController<AISMLobbyController>())
		{
			FString ChapterName = FString::Printf(TEXT("Chapter%d"), GI->CurrChapterNo);
			LobbyController->CreateSession(FName(*ChapterName));
		}
	}	
}
