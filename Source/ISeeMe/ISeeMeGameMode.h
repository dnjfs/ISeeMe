// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include <Interfaces/OnlineSessionDelegates.h>
#include "ISeeMeGameMode.generated.h"

UCLASS(minimalapi)
class AISeeMeGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AISeeMeGameMode();

	/*For give each player a number*/
	void PostLogin(APlayerController* NewPlayer);

	/*For swap camera*/
	void SwapCamera();

	bool bSwapCamera = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Swap")
	float SwapTime = 30;

	FTimerHandle SwapTimerHandle;

	UPROPERTY()
	TArray<TSubclassOf<APawn>> SelectedPawnClasses;

	void ChangePawn();

	int SelectNum = 0;

	int ReadEnding = 0;

	UFUNCTION()
	void CountReadEnding();

	UFUNCTION()
	virtual void OnDestroySessionComplete(FName SessionName, bool bWasSuccessful);

private:
	FOnDestroySessionCompleteDelegate OnDestroySessionCompleteDelegate;
};



