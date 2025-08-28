// Fill out your copyright notice in the Description page of Project Settings.


#include "ISMCharacterState.h"
#include <Net/UnrealNetwork.h>
#include "ISMGameInstance.h"
#include "ISMPlayerController.h"
#include "ISeeMeCharacter.h"
#include "ISeeMeGameMode.h"
#include "ISMLobbyController.h"

UStaticMeshComponent* AISMCharacterState::GetRespawnPoint(int InCustomPlayerId)
{
	if (!CurCheckPoint.IsValid())
		return nullptr;
	
	UE_LOG(LogTemp, Warning, TEXT("%d"), InCustomPlayerId);

	if (InCustomPlayerId == 1)
		return CurCheckPoint->Spawn1PPlayer;
	else if (InCustomPlayerId == 2)
		return CurCheckPoint->Spawn2PPlayer;

	return nullptr;
}

void AISMCharacterState::SetCurCheckPoint(AISMCheckPoint* InCheckPoint)
{
	bCheckPoint = false;
	CurCheckPoint = InCheckPoint;
}
