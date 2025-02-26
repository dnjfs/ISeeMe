// Fill out your copyright notice in the Description page of Project Settings.


#include "ISMGameState.h"
#include <Net/UnrealNetwork.h>
#include <Kismet/GameplayStatics.h>

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

void AISMGameState::MulticastSetSwapViewItem_Implementation(AISMSwapViewItem* AcqSwapViewItem)
{
	SwapViewItem = AcqSwapViewItem;

	bool bShowIcon = (SwapViewItem != nullptr);
	if (OnSwapItemUpdated.IsBound())
		OnSwapItemUpdated.Execute(bShowIcon); // Call Delegate
}

void AISMGameState::MulticastReturnSwapViewItem_Implementation()
{
	// Used Item Empty
	for (auto& Item : UsedSwapViewItems)
	{
		Item->MulticastVisibleMesh(true);
		Item = nullptr;
	}
	UsedSwapViewItems.Empty();

	// Return current swap item 
	if (SwapViewItem != nullptr)
	{
		SwapViewItem->MulticastVisibleMesh(true);
		MulticastSetSwapViewItem(nullptr);
	}

	// Return Check point state
	if (bAcqCheckPoint && SaveSwapViewItem != nullptr)
	{
		SaveSwapViewItem->MulticastVisibleMesh(false);
		MulticastSetSwapViewItem(SaveSwapViewItem);
	}
}

