// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "ISMHUD.generated.h"

class UInGameMenu;

UCLASS()
class ISEEME_API AISMHUD : public AHUD
{
	GENERATED_BODY()
	
public:
	void ToggleInGameMenu(APlayerController* PlayerController);

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UInGameMenu> InGameMenuClass;

	UPROPERTY()
	UInGameMenu* InGameMenu;
};
