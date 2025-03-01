// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ISMLobby.generated.h"

class UButton;
class UISMChapterSelect;
/**
 * 
 */
UCLASS()
class ISEEME_API UISMLobby : public UUserWidget
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
	UISMChapterSelect* WBP_ISMChapterSelect;
};
