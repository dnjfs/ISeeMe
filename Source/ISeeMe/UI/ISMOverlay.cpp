// Fill out your copyright notice in the Description page of Project Settings.

#include "ISeeMe/UI/ISMOverlay.h"
#include "ISMOverlay.h"
#include "Components/CanvasPanel.h"
#include "Components/WidgetSwitcher.h" 
#include "ISeeMe/ISMGameState.h"
#include <ISeeMe/ISMChapterClearTrigger.h>
#include <Kismet/GameplayStatics.h>
#include <ISeeMe/ISMLobbyController.h>
#include "ISeeMe/ISMChapterEndClearTrigger.h"

void UISMOverlay::NativeConstruct()
{
	Super::NativeConstruct();

	if (AISMGameState* GS = GetWorld()->GetGameState<AISMGameState>())
	{
		GS->OnSwapItemUpdated.BindLambda([this](bool bHasItem) {
			if (ItemWidgetSwitcher)
			{
				ItemWidgetSwitcher->SetActiveWidgetIndex(bHasItem ? 1 : 0);
			}
		});
	}

	UWorld* World = GetWorld();
	if (!World) return;

	AISMChapterClearTrigger* ChapterClearTrigger = Cast<AISMChapterClearTrigger>(
		UGameplayStatics::GetActorOfClass(World, AISMChapterClearTrigger::StaticClass())
	); // Find AISMChapterClearTrigger

	AISMChapterEndClearTrigger* ChapterEndClearTrigger = Cast<AISMChapterEndClearTrigger>(
		UGameplayStatics::GetActorOfClass(World, AISMChapterEndClearTrigger::StaticClass())
	); // Find AISMChapterEndClearTrigger

	if (ChapterClearTrigger)
	{
		ChapterClearTrigger->OnClearUpdated.BindUObject(this, &UISMOverlay::HandleClearUpdated);
	} // OnClearUpdated Delegate Bind

	if (ChapterEndClearTrigger)
	{
		ChapterEndClearTrigger->OnClearEndUpdated.BindUObject(this, &UISMOverlay::HandleEndClearUpdated);
	} // OnClearEndUpdated Delegate Bind
}

void UISMOverlay::HandleClearUpdated(bool bLoading)
{
	if (LoadingSwitcher)
	{
		LoadingSwitcher->SetActiveWidgetIndex(bLoading ? 1 : 0);
	}
}

void UISMOverlay::HandleEndClearUpdated(bool bEnding)
{
	if(GetOwningLocalPlayer() && LoadingSwitcher)
	{
		if (bEnding)
		{
			LoadingSwitcher->SetActiveWidgetIndex(2);
			if (UISMPrologue* PrologueWidget = Cast<UISMPrologue>(LoadingSwitcher->GetActiveWidget()))
			{
				PrologueWidget->ShowPrologue();
			}
		}
	}
}
