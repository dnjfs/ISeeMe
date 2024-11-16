// Fill out your copyright notice in the Description page of Project Settings.


#include "ISeeMe/UI/ISMHUD.h"
#include "ISeeMe/UI/InGameMenu.h"

void AISMHUD::BeginPlay()
{
	Super::BeginPlay();

	if (UWorld* World = GetWorld())
	{
		APlayerController* PlayerController = World->GetFirstPlayerController();
		if (PlayerController && InGameMenuClass)
		{
			InGameMenu = CreateWidget<UInGameMenu>(PlayerController, InGameMenuClass);
			InGameMenu->AddToViewport();
		}
	}
}

void AISMHUD::ToggleInGameMenu(APlayerController* PlayerController)
{
	InGameMenu->ToggleWidget(PlayerController);
}
