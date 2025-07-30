// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "ISMPrologue.h"
#include "ISMHUD.generated.h"

class UInGameMenu;
class UISMOverlay;

UCLASS()
class ISEEME_API AISMHUD : public AHUD
{
	GENERATED_BODY()
	
public:
	void ToggleInGameMenu(APlayerController* PlayerController);

	FORCEINLINE UISMOverlay* GetISMOverlay() { return ISMOverlay; };

	UFUNCTION()
	void InitWidgets();

	UFUNCTION()
	void RemoveWidgets();

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UInGameMenu> InGameMenuClass;

	UPROPERTY()
	UInGameMenu* InGameMenu;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UISMOverlay> ISMOverlayClass;

	UPROPERTY()
	UISMOverlay* ISMOverlay;
};
