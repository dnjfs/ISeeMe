// Fill out your copyright notice in the Description page of Project Settings.


#include "ISMGameState.h"
#include <Net/UnrealNetwork.h>
#include <Kismet/GameplayStatics.h>
#include "Components/AudioComponent.h"

AISMGameState::AISMGameState()
{
	bReplicates = true;
	SwapViewItem = nullptr;
	SaveSwapViewItem = nullptr;
	UsedSwapViewItems.Empty();

	static ConstructorHelpers::FObjectFinder<USoundBase> ItemSoundRef(TEXT("/Script/Engine.SoundWave'/Game/ISeeMe/Sounds/SFX/SFX_ItemTimer.SFX_ItemTimer'"));
	if (ItemSoundRef.Object)
	{
		ItemTimerSound = ItemSoundRef.Object;
	}
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
	if (SaveSwapViewItem != nullptr)
	{
		SaveSwapViewItem->MulticastVisibleMesh(false);
		MulticastSetSwapViewItem(SaveSwapViewItem);
	}

	MulticastPlayItemSound(false);
}

void AISMGameState::OnItemUsed(AISMSwapViewItem* InItem)
{
	// サーバーでのみ実行される関数
	if (GetNetMode() == ENetMode::NM_Client)
		return;

	UsedSwapViewItems.Add(InItem);
	MulticastSetSwapViewItem(nullptr);
	MulticastPlayItemSound(true);
}

void AISMGameState::MulticastPlayItemSound_Implementation(bool bIsPlay)
{
	if (bIsPlay)
	{
		TimerAudioComponent = UGameplayStatics::SpawnSound2D(this, ItemTimerSound);
	}
	else
	{
		if (TimerAudioComponent)
			TimerAudioComponent->Stop();
	}
}

