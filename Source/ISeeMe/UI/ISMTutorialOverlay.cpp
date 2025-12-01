// Fill out your copyright notice in the Description page of Project Settings.


#include "ISMTutorialOverlay.h"
#include "ISeeMe/ISMTutorialGameState.h"
#include "Components/WidgetSwitcher.h" 
#include "Components/TextBlock.h" 
#include "Internationalization/StringTableRegistry.h"
#include <ISeeMe/ISMChapterClearTrigger.h>
#include <Kismet/GameplayStatics.h>

void UISMTutorialOverlay::NativeConstruct()
{
	Super::NativeConstruct();

	if (AISMTutorialGameState* GS = GetWorld()->GetGameState<AISMTutorialGameState>())
	{
		GS->OnTutorialUpdated.BindLambda([this](bool bInformation, int TutorialState) {
			if (TutorialtSwitcher)
			{
				if (bInformation)
				{
					TutorialtSwitcher->SetActiveWidgetIndex(0);
					
					MyFunction(TutorialState, 0);
				}
				else
				{
					TutorialtSwitcher->SetActiveWidgetIndex(1);
				}
			}
		});
	} 

	MyFunction(0, 0);
}

void UISMTutorialOverlay::MyFunction(int TutorialStep, int InformationIdx)
{
	FTimerHandle InformationTimerHandle;

	FString Key = FString::Printf(TEXT("Information_%d_%d"), TutorialStep, InformationIdx);
	FText Text = FText::FromStringTable(FName("/Game/ISeeMe/Strings/ST_Tutorial"), Key);

	if (Text.ToString().Contains(TEXT("MISSING STRING TABLE ENTRY"))) // Current Information End
	{
		NextFunction(TutorialStep);
	}
	else
	{
		TutorialInformationText->SetText(Text);

		TWeakObjectPtr<UISMTutorialOverlay> WeakThis(this);
		GetWorld()->GetTimerManager().SetTimer(InformationTimerHandle, [WeakThis, TutorialStep, InformationIdx]() {
			// this が破棄されたか PendingKill 状態なら nullptr を返す
			if (TStrongObjectPtr<UISMTutorialOverlay> StrongThis = WeakThis.Pin(false))
			{
				StrongThis->MyFunction(TutorialStep, InformationIdx+1);
			}
		}, 3, false);
	}
}

void UISMTutorialOverlay::NextFunction(int TutorialStep)
{
	FString Key = FString::Printf(TEXT("Practice_%d"), TutorialStep);
	FText Text = FText::FromStringTable(FName("/Game/ISeeMe/Strings/ST_Tutorial"), Key);

	TutorialPracticeText->SetText(Text);

	if (AISMTutorialGameState* GS = GetWorld()->GetGameState<AISMTutorialGameState>())
	{
		GS->Practice();
	}
}
