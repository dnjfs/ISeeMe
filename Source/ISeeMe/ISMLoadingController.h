// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "ISMLoadingController.generated.h"

#define	LOG_SCREEN(Format, ...) \
	if (GEngine)\
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT(Format), ##__VA_ARGS__))

/**
 * 
 */
DECLARE_DELEGATE_TwoParams(FOnCreateSessionCompleteDelegate, FName /*SessionName*/, bool /*bWasSuccessful*/);
DECLARE_DELEGATE_TwoParams(OnDestroySessionComplete, FName /*SessionName*/, bool /*bWasSuccessful*/);

UCLASS()
class ISEEME_API AISMLoadingController : public APlayerController
{
	GENERATED_BODY()
	
	AISMLoadingController();

public:
	UFUNCTION(BlueprintCallable)
	void CreateSession(FName ChapterName);
	UFUNCTION()
	void StartCreatingSession(FName ChapterName);

protected:
	virtual void BeginPlay() override;

	void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);
	void OnDestroySessionComplete(FName SessionName, bool bWasSuccessful);
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadwrite, Category = UI)
	TSubclassOf<class UUserWidget> UILoadingClass;

private:
	UPROPERTY()
	TObjectPtr<class UUserWidget> UILoadingInstance;

	IOnlineSessionPtr OnlineSessionInterface;
	FOnCreateSessionCompleteDelegate CreateSessionCompleteDelegate;
	FOnDestroySessionCompleteDelegate DestroySessionCompleteDelegate;
};
