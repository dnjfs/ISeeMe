// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ISMGameState.h"
#include "ISMTutorialGameState.generated.h"

/**
 * 
 */
#define	LOG_SCREEN(Format, ...) \
	if (GEngine)\
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT(Format), ##__VA_ARGS__))

DECLARE_DELEGATE_TwoParams(FOnTutorialUpdated, bool, int);

UCLASS()
class ISEEME_API AISMTutorialGameState : public AISMGameState
{
	GENERATED_BODY()

protected:
	AISMTutorialGameState();

public:
	FOnTutorialUpdated OnTutorialUpdated;

	UPROPERTY(Replicated)
	bool bInformation = true; 

	UPROPERTY(Replicated)
	int TutorialStep = 0;
	
	/*When Practice, Call Method*/
	UFUNCTION()
	void Practice();

	/*When Information, Call Method*/
	UFUNCTION(NetMulticast, Reliable,BlueprintCallable)
	void MulticastInformation();

private:
	UFUNCTION()
	void SendTutorialState();
};
