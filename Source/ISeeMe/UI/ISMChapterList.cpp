// Fill out your copyright notice in the Description page of Project Settings.


#include "ISMChapterList.h"
#include "ISeeMe/ISMGameInstance.h"
#include "ISeeMe/ISMSaveGame.h"
#include "Kismet/GameplayStatics.h"
#include "UMG.h"
#include "ISeeMe/ISMLobbyController.h"
#include <ISeeMe/ISMLobbyGameMode.h>

void UISMChapterList::EnableChapterSelectButton(int MaxChapterNo)
{
	for (int i = 0; i < MaxChapterNo; i++)
	{
		if (UWidget* ChapterButton = ChapterListBox->GetChildAt(i))
		{
			ChapterButton->SetIsEnabled(true);
		}
	}
}

void UISMChapterList::SelectChapter(int ChapterNo)
{
	if (UISMGameInstance* GI = GetGameInstance<UISMGameInstance>())
	{
		// 現在のチャプターとチェックポイント情報を初期化して保存
		GI->CurrChapterNo = ChapterNo;
		GI->SavedCheckPointID = FName("None");

		GI->SaveGame();
		GI->bTutorial = true;

		FString ChapterName = FString::Printf(TEXT("Chapter%d"), GI->CurrChapterNo);
		if (AISMLobbyGameMode* GM = Cast<AISMLobbyGameMode>(GetWorld()->GetAuthGameMode()))
		{
			GM->SelectCharacterUI();
		}
	}
}
