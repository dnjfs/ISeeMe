// Fill out your copyright notice in the Description page of Project Settings.


#include "ISMTutorialHUD.h"

void AISMTutorialHUD::BeginPlay()
{
	Super::BeginPlay();

	APlayerController* PC = GetOwningPlayerController();
	if (PC == nullptr)
		return;

	Tutorial = CreateWidget<UUserWidget>(PC, TutorialClass);
	if (Tutorial)
	{
		Tutorial->AddToViewport();
	}
}

