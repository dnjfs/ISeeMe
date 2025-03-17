// Fill out your copyright notice in the Description page of Project Settings.


#include "ISMLobbyGameState.h"

void AISMLobbyGameState::InitSelect()
{
	for (int i = 0; i < 2; i++)
	{
		bSelectedHojin[i] = false;
		bSelectedMimi[i] = false;
	}
}
