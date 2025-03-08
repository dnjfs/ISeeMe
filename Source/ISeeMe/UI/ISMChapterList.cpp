// Fill out your copyright notice in the Description page of Project Settings.


#include "ISMChapterList.h"
#include "ISeeMe/ISMGameInstance.h"
#include "ISeeMe/ISMSaveGame.h"
#include "Kismet/GameplayStatics.h"
#include "UMG.h"
#include "ISMStartController.h"
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
		// 현재 챕터, 체크포인트 정보 초기화 후 저장
		GI->CurrChapterNo = ChapterNo;
		GI->SavedCheckPointID = FName("None");

		GI->SaveGame();

		if (AISMStartController* StartController = GetWorld()->GetFirstPlayerController<AISMStartController>())
		{
			FString ChapterName = FString::Printf(TEXT("Chapter%d"), GI->CurrChapterNo);
			GI->SelectChapter = FName(*ChapterName);
			if(AISMLobbyGameMode* GM = Cast<AISMLobbyGameMode>(GetWorld()->GetAuthGameMode()))
				GM->ChangeSelectCharacter();
			//LobbyController->CreateSession(FName(*ChapterName));
		}
	}	
}
