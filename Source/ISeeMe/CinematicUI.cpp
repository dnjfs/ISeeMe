// Fill out your copyright notice in the Description page of Project Settings.


#include "CinematicUI.h"
#include "Components/ProgressBar.h"

void UCinematicUI::SetSkip(float CurrentProgress, float MaxProgress)
{
	if (playerBar)
	{
		playerBar->SetPercent(CurrentProgress);
		UE_LOG(LogTemp, Warning, TEXT("%f"), playerBar->GetPercent());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Not Bar"));
	}
}
