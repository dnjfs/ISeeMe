// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include <Components/WidgetSwitcher.h>
#include <Components/TextBlock.h>
#include "ISMPrologue.generated.h"

#define	LOG_SCREEN(Format, ...) \
	if (GEngine)\
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT(Format), ##__VA_ARGS__))
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
