// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ISMPlayerController.h"
#include "ISMTutorialController.generated.h"

#define	LOG_SCREEN(Format, ...) \
	if (GEngine)\
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT(Format), ##__VA_ARGS__))

/**
 * 
 */
UCLASS()
class ISEEME_API AISMTutorialController : public AISMPlayerController
{
	GENERATED_BODY()

public:
	void ServerCallSwapCamera() override;

protected:
	virtual void BeginPlay();
	
private:
	void UseItemPractice();
};
