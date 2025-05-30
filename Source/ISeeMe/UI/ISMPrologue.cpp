// Fill out your copyright notice in the Description page of Project Settings.


#include "ISMPrologue.h"
#include <ISeeMe/ISMGameInstance.h>
#include <ISeeMe/ISMLobbyController.h>

void UISMPrologue::ShowPrologue()
{
	LOG_SCREEN("Show Prologue");

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

	if (AISMLobbyController* Controller = Cast<AISMLobbyController>(GetOwningPlayer()))
	{
		FInputModeUIOnly Mode;
		Mode.SetWidgetToFocus(this->GetCachedWidget());
		Controller->SetInputMode(Mode);
	}
}

void UISMPrologue::ReadPrologue()
{
	SkipText->SetText(FText::FromString("Wait a minute"));
	if (AISMLobbyController* Controller = Cast<AISMLobbyController>(GetOwningPlayer()))
	{
		Controller->CallReadTutorial();
	}
}
