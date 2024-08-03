// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "ISMLobbyController.generated.h"

#define	LOG_SCREEN(Format, ...) \
	if (GEngine)\
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT(Format), ##__VA_ARGS__))

DECLARE_DELEGATE_TwoParams(FOnCreateSessionCompleteDelegate, FName /*SessionName*/, bool /*bWasSuccessful*/);

/**
 * 
 */
UCLASS()
class ISEEME_API AISMLobbyController : public APlayerController
{
	GENERATED_BODY()

	AISMLobbyController();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadwrite, Category = UI)
	TSubclassOf<class UUserWidget> UIWidgetClass;

private:
	UPROPERTY()
	TObjectPtr<class UUserWidget> UIWidgetInstance;

// Online Subsystem
protected:
	bool GetSessionInterface();

	UFUNCTION(BlueprintCallable)
	void CreateSession();

	void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);

private:
	IOnlineSessionPtr OnlineSessionInterface;

	FOnCreateSessionCompleteDelegate CreateSessionComplete;
};
