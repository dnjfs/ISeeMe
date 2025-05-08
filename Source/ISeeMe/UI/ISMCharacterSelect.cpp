// Fill out your copyright notice in the Description page of Project Settings.


#include "ISMCharacterSelect.h"
#include <Kismet/GameplayStatics.h>
#include <ISeeMe/ISMGameInstance.h>
#include <ISeeMe/ISMLobbyController.h>
#include "ISeeMe/ISMLobbyGameState.h"
#include <ISeeMe/ISMLobbyGameMode.h>

void UISMCharacterSelect::NativeConstruct()
{
	Super::NativeConstruct();

	ApplyButton->SetVisibility(ESlateVisibility::Hidden);

	ApplyButton->OnClicked.AddDynamic(this, &UISMCharacterSelect::MoveLevel);
	BackButton->OnClicked.AddDynamic(this, &UISMCharacterSelect::BackSelect);
	HojinButton->OnClicked.AddDynamic(this, &UISMCharacterSelect::HojinSelect);
	MimiButton->OnClicked.AddDynamic(this, &UISMCharacterSelect::MimiSelect);
}

void UISMCharacterSelect::HojinSelect()
{
	if (AISMLobbyController* Controller = Cast<AISMLobbyController>(GetOwningPlayer()))
	{
		Controller->CallChangeCharacterButton("Hojin");
		HojinButton->SetIsEnabled(false);
		MimiButton->SetIsEnabled(true);
	}
}

void UISMCharacterSelect::MimiSelect()
{
	if (AISMLobbyController* Controller = Cast<AISMLobbyController>(GetOwningPlayer()))
	{
		Controller->CallChangeCharacterButton("Mimi");
		HojinButton->SetIsEnabled(true);
		MimiButton->SetIsEnabled(false);
	}
}

void UISMCharacterSelect::BackSelect()
{
	if (AISMLobbyController* Controller = Cast<AISMLobbyController>(GetOwningPlayer()))
	{
		Controller->CallChangeCharacterButton("None");
		Controller->CallBackUI("Chapter");
	}
}

void UISMCharacterSelect::MoveLevel()
{
	if (UISMGameInstance* GI = GetGameInstance<UISMGameInstance>())
	{
		FString ChapterName = FString::Printf(TEXT("Chapter%d"), GI->CurrChapterNo);
		UWorld* World = GetWorld();

		if (!GI->bTutorial)
		{
			if (AISMLobbyGameMode* GM = Cast<AISMLobbyGameMode>(GetWorld()->GetAuthGameMode()))
			{
				GM->ChangeTutorial();
			}
		} // Go Tutorial
		else
		{
			if (AISMLobbyGameMode* GM = Cast<AISMLobbyGameMode>(GetWorld()->GetAuthGameMode()))
			{
				GM->LoadingNextLevel();
			}
		} // Go Next Server
	}
}

void UISMCharacterSelect::VisibleApply()
{
	if (AISMLobbyGameState* GS = Cast<AISMLobbyGameState>(GetWorld()->GetGameState()))
	{
		if ((GS->bSelectedHojin[0] && GS->bSelectedMimi[1]) 
			|| (GS->bSelectedHojin[1] && GS->bSelectedMimi[0]))
		{
			ApplyButton->SetVisibility(ESlateVisibility::Visible);
		}
		else
		{
			ApplyButton->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

void UISMCharacterSelect::InitButton()
{
	HojinButton->SetIsEnabled(true);
	MimiButton->SetIsEnabled(true);
	HojinButton->SetBackgroundColor(FLinearColor(1, 1, 1, 0.1f));
	MimiButton->SetBackgroundColor(FLinearColor(1, 1, 1, 0.1f));
}
