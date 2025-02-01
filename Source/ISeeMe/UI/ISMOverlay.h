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
	void OffSelectPanel();

protected:
	virtual void NativeConstruct() override;

private:
	UPROPERTY(meta = (BindWidget))
	class UCanvasPanel* SwapItemPanel;

	UPROPERTY(meta = (BindWidget))
	class UCanvasPanel* CharacterSelectPanel;

};
