// Fill out your copyright notice in the Description page of Project Settings.


#include "ISMLobbyMenu.h"
#include <ISeeMe/ISMLobbyController.h>

void UISMLobbyMenu::ChangeLobbyUI()
{
	LobbySwitcher->SetActiveWidgetIndex(0);
}

void UISMLobbyMenu::ChangeInviteFriendUI()
{
	LobbySwitcher->SetActiveWidget(LobbySwitcher->GetWidgetAtIndex(1));
	LobbySwitcher->GetWidgetAtIndex(0)->SetVisibility(ESlateVisibility::Hidden);
	//LobbySwitcher->SetActiveWidgetIndex(1);
}

void UISMLobbyMenu::ChangeChpaterServer()
{
	LOG_SCREEN("CallSelectChapterUI");
	LobbySwitcher->SetActiveWidget(LobbySwitcher->GetWidgetAtIndex(2));
}

void UISMLobbyMenu::ChangeChapterClient()
{
	LOG_SCREEN("ChangeChapterClient");
	LobbySwitcher->SetActiveWidget(LobbySwitcher->GetWidgetAtIndex(3));
}

void UISMLobbyMenu::ChangeCaracterUI()
{
	LobbySwitcher->SetActiveWidgetIndex(4);
}
