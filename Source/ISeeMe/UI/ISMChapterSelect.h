// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ISMChapterSelect.generated.h"

class UButton;
class UISMChapterList;
/**
 * 
 */
UCLASS()
class ISEEME_API UISMChapterSelect : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;

	UFUNCTION()
	void NewGame();
	UFUNCTION()
	void Continue();

private:
	UPROPERTY(meta = (BindWidget))
	UButton* NewGameButton;

	UPROPERTY(meta = (BindWidget))
	UButton* ContinueButton;

	UPROPERTY(meta = (BindWidget))
	UButton* BackButton;

	UPROPERTY(meta = (BindWidget))
	UISMChapterList* WBP_ISMChapterList;
};
