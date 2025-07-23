// Fill out your copyright notice in the Description page of Project Settings.


#include "ISMCharacterState.h"
#include <Net/UnrealNetwork.h>
#include "ISMGameInstance.h"
#include "ISMPlayerController.h"
#include "ISeeMeCharacter.h"
#include "ISeeMeGameMode.h"
#include "ISMLobbyController.h"

void AISMCharacterState::BeginPlay()
{
	if (HasActorBegunPlay())
	{
		return;
	}

	if (UISMGameInstance* GameInstance = Cast<UISMGameInstance>(GetGameInstance()))
	{
		CallSelectPawn(GameInstance->SelectedPawnClass);
	}

	Super::BeginPlay();
}

UStaticMeshComponent* AISMCharacterState::GetRespawnPoint(int InCustomPlayerId)
{
	if (CurCheckPoint == nullptr)
		return nullptr;

	if (InCustomPlayerId == 1)
		return CurCheckPoint->Spawn1PPlayer;
	else if (InCustomPlayerId == 2)
		return CurCheckPoint->Spawn2PPlayer;

	return nullptr;
}

void AISMCharacterState::CallSelectPawn(TSubclassOf<APawn> NewPawn)
{
	if (HasAuthority())
	{
		SelectPawn(NewPawn, 0);
	}
	else 
	{
		APlayerController* PC = Cast<APlayerController>(GetWorld()->GetFirstPlayerController());

		if (!PC)
		{
			PC = Cast<APlayerController>(GetOuter()); // 현재 객체의 Owner 확인
		}
		if (!PC)
		{
			PC = GetNetOwningPlayer()->GetPlayerController(GetWorld()); // 네트워크 소유자를 통한 방식
		}
		if (PC && PC->IsLocalController())
		{
			ServerSelectPawn(NewPawn);
		}
	}
}

void AISMCharacterState::SelectPawn(TSubclassOf<APawn> NewPawn, int num)
{
	if (AISeeMeGameMode* GM = Cast<AISeeMeGameMode>(GetWorld()->GetAuthGameMode()))
	{
		if(NewPawn!=nullptr)
			GM->SelectedPawnClasses[num] = NewPawn;

		GM->ChangePawn();
	}
}

void AISMCharacterState::ServerSelectPawn_Implementation(TSubclassOf<APawn> NewPawn)
{
	SelectPawn(NewPawn, 1);
}
