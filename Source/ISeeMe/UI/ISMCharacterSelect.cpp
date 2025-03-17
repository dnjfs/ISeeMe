// Fill out your copyright notice in the Description page of Project Settings.


#include "ISMCharacterSelect.h"
#include <Kismet/GameplayStatics.h>
#include <ISeeMe/ISMGameInstance.h>
#include <ISeeMe/ISMLobbyController.h>
#include "ISeeMe/ISMLobbyGameState.h"

void UISMCharacterSelect::NativeConstruct()
{
	Super::NativeConstruct();

	ApplyButton->SetVisibility(ESlateVisibility::Hidden);

	ApplyButton->OnClicked.AddDynamic(this, &UISMCharacterSelect::MoveLevel);
	HojinButton->OnClicked.AddDynamic(this, &UISMCharacterSelect::HojinSelect);
	MimiButton->OnClicked.AddDynamic(this, &UISMCharacterSelect::MimiSelect);
}

void UISMCharacterSelect::HojinSelect()
{
	if (AISMLobbyGameState* GS = Cast<AISMLobbyGameState>(GetWorld()->GetGameState()))
	{
		AISMLobbyController* Controller = Cast<AISMLobbyController>(GetOwningPlayer());

		if (Controller == nullptr)
			return;

		Controller->CallChangeCharacterButton("Hojin");
		HojinButton->SetIsEnabled(false);
		MimiButton->SetIsEnabled(true);
	}
}

void UISMCharacterSelect::MimiSelect()
{
	if (AISMLobbyGameState* GS = Cast<AISMLobbyGameState>(GetWorld()->GetGameState()))
	{
		AISMLobbyController* Controller = Cast<AISMLobbyController>(GetOwningPlayer());

		if (Controller == nullptr)
			return;

		Controller->CallChangeCharacterButton("Mimi");
		HojinButton->SetIsEnabled(true);
		MimiButton->SetIsEnabled(false);
	}
}

void UISMCharacterSelect::MoveLevel()
{
	if (UISMGameInstance* GI = GetGameInstance<UISMGameInstance>())
	{
		FString ChapterName = FString::Printf(TEXT("Chapter%d"), GI->CurrChapterNo);
		UWorld* World = GetWorld();

		World->ServerTravel("/Game/ISeeMe/Maps/" + ChapterName + "?listen", true);
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
