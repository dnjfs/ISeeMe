// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ISMUIGuide.generated.h"

USTRUCT()
struct FStringTableKeyPair
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	FText Key;

	UPROPERTY(EditAnywhere)
	FText Value;
};

/**
 * 
 */
UCLASS()
class ISEEME_API UISMUIGuide : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetPage(int32 InPageNum);

	UFUNCTION()
	void OnClickedPrevButton();

	UFUNCTION()
	void OnClickedNextButton();
	
protected:
	virtual void NativeOnInitialized() override;

private:
	UPROPERTY(EditDefaultsOnly)
	TArray<FStringTableKeyPair> GuideTextList;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> SubjectText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> GuideText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> PrevButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> NextButton;

	int32 PageNum = 0;
};
