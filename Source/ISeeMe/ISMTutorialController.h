// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ISMPlayerController.h"
#include "ISMTutorialController.generated.h"

/**
 * 
 */
UCLASS()
class ISEEME_API AISMTutorialController : public AISMPlayerController
{
	GENERATED_BODY()

protected:
	virtual void SetPawn(APawn* aPawn) override;

public:
	virtual void SwapCamera() override;
};
