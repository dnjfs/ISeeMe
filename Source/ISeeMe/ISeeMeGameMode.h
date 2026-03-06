// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include <Interfaces/OnlineSessionDelegates.h>
#include "ISeeMeGameMode.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCharacterReturned);

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
	void RestoreCamera();

	void ReturnCharacters();

	bool bSwapCamera = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Swap")
	float SwapTime = 30;

	FTimerHandle SwapTimerHandle;

	bool ChangePawn(APlayerController* Controller, TSubclassOf<APawn> SelectedPawnClass);

	int SelectNum = 0;

	int ReadEnding = 0;

	virtual void Logout(AController* Exiting) override;

	UFUNCTION()
	void CountReadEnding();

	UFUNCTION()
	virtual void OnDestroySessionComplete(FName SessionName, bool bWasSuccessful);

	UPROPERTY(BlueprintAssignable)
	FOnCharacterReturned OnCharacterReturned;

private:
	FOnDestroySessionCompleteDelegate OnDestroySessionCompleteDelegate;

	bool bFirst = true;
};



