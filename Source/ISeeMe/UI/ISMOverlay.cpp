// Fill out your copyright notice in the Description page of Project Settings.

#include "ISeeMe/UI/ISMOverlay.h"
#include "ISMOverlay.h"
#include "Components/CanvasPanel.h"
#include "ISeeMe/ISMGameState.h"

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
}

void UISMOverlay::OffSelectPanel()
{
	CharacterSelectPanel->SetVisibility(ESlateVisibility::Hidden);
}
