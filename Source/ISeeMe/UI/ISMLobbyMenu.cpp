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
}

void UISMLobbyMenu::ChangeChpaterServer()
{
	LobbySwitcher->SetActiveWidget(LobbySwitcher->GetWidgetAtIndex(2));
}

void UISMLobbyMenu::ChangeChapterClient()
{
	LobbySwitcher->SetActiveWidget(LobbySwitcher->GetWidgetAtIndex(3));
}

void UISMLobbyMenu::ChangeCaracterUI()
{
	LobbySwitcher->SetActiveWidgetIndex(4);
}
