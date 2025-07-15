// Fill out your copyright notice in the Description page of Project Settings.


#include "ISeeMe/UI/ISMHUD.h"
#include "ISeeMe/UI/InGameMenu.h"
#include "ISeeMe/UI/ISMOverlay.h"
#include <ISeeMe/ISMLobbyController.h>
#include <ISeeMe/ISMPlayerController.h>

void AISMHUD::BeginPlay()
{
	Super::BeginPlay();
    
	UE_LOG(LogTemp, Warning, TEXT("HUD BeginPlay"));
	InitWidgets();
}

void AISMHUD::InitWidgets()
{
	if (UWorld* World = GetWorld())
	{
		APlayerController* PlayerController = GetOwningPlayerController();
		if (PlayerController->IsLocalController())
		{
			if (PlayerController && InGameMenuClass)
			{
				InGameMenu = CreateWidget<UInGameMenu>(PlayerController, InGameMenuClass);
				InGameMenu->AddToViewport(1);
			}
			if (PlayerController && ISMOverlayClass)
			{
				ISMOverlay = CreateWidget<UISMOverlay>(PlayerController, ISMOverlayClass);
				ISMOverlay->AddToViewport(0);
			}
		}
	}
}

void AISMHUD::ToggleInGameMenu(APlayerController* PlayerController)
{
	if (PlayerController->IsLocalController())
	{
		InGameMenu->ToggleWidget(PlayerController);
	}
}

