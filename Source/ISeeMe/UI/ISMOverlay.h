// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ISMOverlay.generated.h"

/**
 * 
 */
UCLASS()
class ISEEME_API UISMOverlay : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	class UWidgetSwitcher* LoadingSwitcher;

	UFUNCTION()
	void HandleClearUpdated(bool bLoading);

protected:
	virtual void NativeConstruct() override;

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UWidgetSwitcher> ItemWidgetSwitcher;

};
