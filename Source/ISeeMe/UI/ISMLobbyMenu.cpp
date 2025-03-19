// Fill out your copyright notice in the Description page of Project Settings.


#include "ISMLobbyMenu.h"
#include <ISeeMe/ISMLobbyController.h>

void UISMLobbyMenu::ChangeLobbyUI(int32 Index)
{
	LobbySwitcher->SetActiveWidget(LobbySwitcher->GetWidgetAtIndex(Index)); 
}
