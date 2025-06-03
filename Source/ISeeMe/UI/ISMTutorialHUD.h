// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ISMHUD.h"
#include "ISMTutorialHUD.generated.h"

/**
 * 
 */
UCLASS()
class ISEEME_API AISMTutorialHUD : public AISMHUD
{
	GENERATED_BODY()
	
protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> TutorialClass;

	UPROPERTY()
	UUserWidget* Tutorial;
};
