// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ISeeMeGameMode.generated.h"

UCLASS(minimalapi)
class AISeeMeGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AISeeMeGameMode();
	void SwapCamera();
};



