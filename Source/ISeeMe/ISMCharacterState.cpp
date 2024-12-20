// Fill out your copyright notice in the Description page of Project Settings.


#include "ISMCharacterState.h"

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
