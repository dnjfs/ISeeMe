// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include <Components/WidgetSwitcher.h>
#include <Components/TextBlock.h>
#include "ISMPrologue.generated.h"

/**
 * 
 */
UCLASS()
class ISEEME_API UISMPrologue : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	UWidgetSwitcher* PrologueSwitcher;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* SkipText;
	
	UFUNCTION(BlueprintCallable)
	void ShowPrologue();

	UFUNCTION(BlueprintCallable)
	void ReadPrologue();
};
