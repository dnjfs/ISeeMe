// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CinematicUI.generated.h"

/**
 * 
 */
UCLASS()
class ISEEME_API UCinematicUI : public UUserWidget
{
	GENERATED_BODY()

private:
	UPROPERTY(meta = (BindWidget))
	class UProgressBar* playerBar;

public:
	UFUNCTION()
	void SetSkip(float CurrentProgress, float MaxProgress);

	UPROPERTY(EditAnywhere)
	float curProgress = 0.0f;

};
