// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ISMTutorialOverlay.generated.h"

/**
 * 
 */
UCLASS()
class ISEEME_API UISMTutorialOverlay : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> TutorialInformationText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> TutorialPracticeText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UWidgetSwitcher> TutorialtSwitcher;

	/*Maybe Letter Implementation?*/
	void MyFunction(int TutorialStep, int InformationIdx);

	/*Turn Practice State*/
	void NextFunction(int TutorialStep);

};
