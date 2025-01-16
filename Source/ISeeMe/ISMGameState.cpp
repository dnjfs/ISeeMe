// Fill out your copyright notice in the Description page of Project Settings.


#include "ISMGameState.h"
#include <Net/UnrealNetwork.h>
#include "ISMGameInstance.h"
#include "ISeeMeCharacter.h"
#include <Kismet/GameplayStatics.h>
#include "ISMPlayerController.h"

AISMGameState::AISMGameState()
{
	bReplicates = true;
	SwapViewItem = nullptr;
	SaveSwapViewItem = nullptr;
	UsedSwapViewItems.Empty();
	bAcqCheckPoint = false;
}

void AISMGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AISMGameState, SwapViewItem);
	DOREPLIFETIME(AISMGameState, SaveSwapViewItem);
	DOREPLIFETIME(AISMGameState, UsedSwapViewItems);
}

