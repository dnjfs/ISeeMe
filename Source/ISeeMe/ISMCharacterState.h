// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "ISMCheckPoint.h"
#include "ISMCharacterState.generated.h"

/**
 * 
 */
UCLASS()
class ISEEME_API AISMCharacterState : public APlayerState
{
	GENERATED_BODY()
	
public:
	/*First Check Point Location*/
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Spawn")
	FVector InitSpawnPointLocation;

	/*First Check Point Rotator*/
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Spawn")
	FRotator InitialSpawnPointRotator;

	/*Current Check Point*/
	AISMCheckPoint* CurCheckPoint;

	/*Current Check Point State*/
	bool bCheckPoint = false;

	/*Player Number*/
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Player Info")
	int32 CustomPlayerID;
};
