// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include <Components/WidgetSwitcher.h>
#include "ISMLobbyMenu.generated.h"

/**
 * 
 */
UCLASS()
class ISEEME_API UISMLobbyMenu : public UUserWidget
{
	GENERATED_BODY()
	
public:

	UFUNCTION(BlueprintCallable)
	void ChangeLobbyUI(int32 Index);

	UPROPERTY(meta = (BindWidget))
	UWidgetSwitcher* LobbySwitcher;
};

