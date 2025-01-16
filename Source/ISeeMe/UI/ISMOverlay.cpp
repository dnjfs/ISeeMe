// Fill out your copyright notice in the Description page of Project Settings.


#include "ISeeMe/UI/ISMOverlay.h"
#include "ISMOverlay.h"
#include "Components/CanvasPanel.h"

void UISMOverlay::SetSwapItemIcon(bool Flag)
{
	if (Flag == true)
	{
		SwapItemPanel->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		SwapItemPanel->SetVisibility(ESlateVisibility::Hidden);
	}
}
