// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "ISMCharacterSelect.generated.h"

/**
 * 
 */
UCLASS()
class ISEEME_API UISMCharacterSelect : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(meta = (BindWidget), BlueprintReadWrite, EditAnywhere)
	UButton* HojinButton;

	UPROPERTY(meta = (BindWidget), BlueprintReadWrite, EditAnywhere)
	UButton* MimiButton;

	UPROPERTY(meta = (BindWidget), BlueprintReadWrite, EditAnywhere)
	UButton* ApplyButton;

	UPROPERTY(meta = (BindWidget), BlueprintReadWrite, EditAnywhere)
	UButton* BackButton;

	UFUNCTION()
	void VisibleApply(); // ApplyButton(StartButton) Visible

	UFUNCTION()
	void InitButton(); // Hojin, Mimi Button Reset

protected:
	virtual void NativeConstruct() override;

	UFUNCTION()
	void HojinSelect();

	UFUNCTION()
	void MimiSelect();

	UFUNCTION()
	void BackSelect();

	UFUNCTION()
	void MoveLevel();
};
