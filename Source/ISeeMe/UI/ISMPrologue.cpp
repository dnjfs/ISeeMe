// Fill out your copyright notice in the Description page of Project Settings.


#include "ISMPrologue.h"
#include <ISeeMe/ISMGameInstance.h>
#include <ISeeMe/ISMLobbyController.h>
#include <ISeeMe/ISMPlayerController.h>

void UISMPrologue::ShowPrologue()
{
	if (UISMGameInstance* GI = GetGameInstance<UISMGameInstance>())
	{
		if (GI->SelectedPawnClass)
		{
			FString ClassName = GI->SelectedPawnClass->GetName();
			if (ClassName == "BP_Mimi_C")
			{
				PrologueSwitcher->SetActiveWidgetIndex(0);
			}
			else if(ClassName == "BP_Boy_C")
			{
				PrologueSwitcher->SetActiveWidgetIndex(1);
			}
		}
	}

	if (AISMLobbyController* LobbyController = Cast<AISMLobbyController>(GetOwningPlayer()))
	{
		FInputModeUIOnly Mode;
		Mode.SetWidgetToFocus(this->GetCachedWidget());
		LobbyController->SetInputMode(Mode);
	}
	else if (AISMPlayerController* Controller = Cast<AISMPlayerController>(GetOwningPlayer()))
	{
		SetKeyboardFocus();
		FInputModeUIOnly Mode;
		Mode.SetWidgetToFocus(this->GetCachedWidget());
		Controller->SetInputMode(Mode);
	}
}

void UISMPrologue::ReadPrologue()
{
	SkipText->SetText(FText::FromString("Wait a minute"));
	if (AISMLobbyController* LobbyController = Cast<AISMLobbyController>(GetOwningPlayer()))
	{
		LobbyController->CallReadTutorial();
	}
	else if (AISMPlayerController* Controller = Cast<AISMPlayerController>(GetOwningPlayer()))
	{
		Controller->CallReadTutorial();
	}
}
