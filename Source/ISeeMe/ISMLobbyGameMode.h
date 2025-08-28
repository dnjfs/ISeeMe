// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ISMLobbyController.h"
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
	UFUNCTION()
	void SelectCharacterUI(); // When two players, Call SelectCharacterUI()

	UFUNCTION()
	void ChangeCharacterButton(FString CharacterSelect,int index); 

	UFUNCTION()
	void BackChapterUI(); // Move to Chapter UI

	bool bTutorial = true; // Have you been Tutorial?

	int ReadTutorial = 0;

	UFUNCTION()
	void CountReadTutorial();

	UFUNCTION()
	void ChangeTutorial();

	UFUNCTION()
	void LoadingNextLevel();

protected:
	virtual void PostLogin(APlayerController* NewPlayer) override;

private:
	UPROPERTY(Transient)
	TArray<AISMLobbyController*> PCs = {nullptr, nullptr};
};
