// Fill out your copyright notice in the Description page of Project Settings.


#include "ISMUIGuide.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Internationalization/StringTableRegistry.h"

void UISMUIGuide::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	PrevButton->OnClicked.AddDynamic(this, &UISMUIGuide::OnClickedPrevButton);
	NextButton->OnClicked.AddDynamic(this, &UISMUIGuide::OnClickedNextButton);

	SetPage(PageNum);
}

void UISMUIGuide::SetPage(int32 InPageNum)
{
	PrevButton->SetVisibility(PageNum <= 0 ? ESlateVisibility::Collapsed : ESlateVisibility::Visible);
	NextButton->SetVisibility(PageNum >= GuideTextList.Num()-1 ? ESlateVisibility::Collapsed : ESlateVisibility::Visible);

	if (!GuideTextList.IsValidIndex(InPageNum))
		return;

	FStringTableKeyPair StringTablePair = GuideTextList[InPageNum];
	
	SubjectText->SetText(StringTablePair.Key);
	GuideText->SetText(StringTablePair.Value);
}

void UISMUIGuide::OnClickedPrevButton()
{
	PageNum = FMath::Clamp(PageNum-1, 0, PageNum);

	SetPage(PageNum);
}

void UISMUIGuide::OnClickedNextButton()
{
	PageNum = FMath::Clamp(PageNum+1, PageNum, GuideTextList.Num()-1);

	SetPage(PageNum);
}