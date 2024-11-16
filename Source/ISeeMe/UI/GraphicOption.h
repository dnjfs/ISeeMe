// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GraphicOption.generated.h"

UENUM(BlueprintType)
enum class EGraphicType : uint8
{
	ANTIALIASING,
	FOLIAGE,
	GLOBALILLUMINATION,
	POSTPROCESSING,
	REFLECTION,
	SHADING,
	SHADOW,
	TEXTURE,
	VIEWDISTANCE,
	VISUALEFFECT,
};

UCLASS()
class ISEEME_API UGraphicOption : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	UFUNCTION()
	virtual void NativeOnInitialized() override;

	UFUNCTION(BlueprintCallable)
	void SetQuality(int32 Value);

	UPROPERTY(EditAnywhere)
	EGraphicType GraphicType = EGraphicType::ANTIALIASING;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* OptionNameText;

private:
	UPROPERTY()
	class UGameUserSettings* GameUserSettings;
};
