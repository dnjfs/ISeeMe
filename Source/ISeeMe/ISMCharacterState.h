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
	AISMCheckPoint* CurCheckPoint;
	bool bCheckPoint = false;

};
