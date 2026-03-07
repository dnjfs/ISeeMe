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

void AISMGameState::OnCharacterReturned()
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
		SwapViewItem = nullptr;
	}

	// Return Check point state
	if (SaveSwapViewItem != nullptr)
	{
		SaveSwapViewItem->MulticastVisibleMesh(false);
		SwapViewItem = SaveSwapViewItem;
	}

	MulticastPlayItemSound(false);
	UpdateItemUIState();
}

void AISMGameState::OnCheckPointSaved()
{
	SaveSwapViewItem = SwapViewItem; // 현재 소지한 아이템은 저장
	UsedSwapViewItems.Empty(); // 사용한 아이템은 초기화 

	UpdateItemUIState();
}

void AISMGameState::OnItemAcquired(AISMSwapViewItem* AcqSwapViewItem)
{
	SwapViewItem = AcqSwapViewItem;

	UpdateItemUIState();
}

void AISMGameState::OnItemUsed()
{
	UsedSwapViewItems.Add(SwapViewItem);
	SwapViewItem = nullptr;

	MulticastPlayItemSound(true);
	UpdateItemUIState();
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

void AISMGameState::UpdateItemUIState()
{
	bool bCanUseItem = (SwapViewItem != nullptr);
	bool bCanRestoreItem = SaveSwapViewItem != nullptr || UsedSwapViewItems.Num() > 0;

	if (OnSwapItemUpdated.IsBound())
		OnSwapItemUpdated.Execute(bCanUseItem, bCanRestoreItem); // Call Delegate
}

void AISMGameState::OnRep_SwapViewItem()
{
	// Replicate 시점과 RPC 호출 시점이 달라 레이스 컨디션이 발생할 수 있음
	// UpdateItemUIState() 함수는 서버에서 갱신 직후 실행하고, 클라이언트에선 OnRep() 함수를 통해 UI 반영
	UpdateItemUIState();
}

bool AISMGameState::HasSwapViewItem() const
{
	if (!HasAuthority())
		return false;

	return SwapViewItem != nullptr;
}
