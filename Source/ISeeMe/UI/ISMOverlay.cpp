// Fill out your copyright notice in the Description page of Project Settings.

#include "ISeeMe/UI/ISMOverlay.h"
#include "ISMOverlay.h"
#include "Components/CanvasPanel.h"
#include "Components/WidgetSwitcher.h" 
#include "ISeeMe/ISMGameState.h"
#include <ISeeMe/ISMChapterClearTrigger.h>
#include <Kismet/GameplayStatics.h>
#include <ISeeMe/ISMLobbyController.h>

void UISMOverlay::NativeConstruct()
{
	Super::NativeConstruct();

	if (AISMGameState* GS = GetWorld()->GetGameState<AISMGameState>())
	{
		GS->OnSwapItemUpdated.BindLambda([this](bool bShowIcon)
			{
				if (SwapItemPanel)
				{
					SwapItemPanel->SetVisibility(bShowIcon ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
				}
			});
	}

	UWorld* World = GetWorld();
	if (!World) return;

	AISMChapterClearTrigger* ChapterClearTrigger = Cast<AISMChapterClearTrigger>(
		UGameplayStatics::GetActorOfClass(World, AISMChapterClearTrigger::StaticClass())
	); // Find AISMChapterClearTrigger

	if (ChapterClearTrigger)
	{
		ChapterClearTrigger->OnClearUpdated.BindUObject(this, &UISMOverlay::HandleClearUpdated);
	}
}

void UISMOverlay::HandleClearUpdated(bool bLoading)
{
	if (LoadingSwitcher)
	{
		LoadingSwitcher->SetActiveWidgetIndex(bLoading ? 1 : 0);
	}
}