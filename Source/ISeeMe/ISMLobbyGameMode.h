// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ISMLobbyGameMode.generated.h"

/**
 * 
 */
UCLASS()
class ISEEME_API AISMLobbyGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	bool bJoin = false;

	void ChangeSelectCharacter();
	
protected:
	virtual void PostLogin(APlayerController* NewPlayer) override;
};
