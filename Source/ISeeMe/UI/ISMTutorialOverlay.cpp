// Fill out your copyright notice in the Description page of Project Settings.


#include "ISMTutorialOverlay.h"
#include "ISeeMe/ISMTutorialGameState.h"
#include "Components/WidgetSwitcher.h" 
#include "Components/TextBlock.h" 

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
					if (TutorialState <= TutorialInformationStrings.Num())
					{
						TutorialInformationText->SetText(FText::FromString(TutorialInformationStrings[TutorialState]));
						MyFunction();
					} // Change Information Canvas and Set Information Text
				}
				else
				{
					TutorialtSwitcher->SetActiveWidgetIndex(1);
					if (TutorialState <= TutorialPracticeStrings.Num())
					{
						TutorialPracticeText->SetText(FText::FromString(TutorialPracticeStrings[TutorialState]));
					} // Change Practice Canvas and Set Practice Text
				}
			}
			});
	} 

	MyFunction();
}

void UISMTutorialOverlay::MyFunction()
{
	FTimerHandle AnyHandle;
	FTimerDelegate AnyDelegate;

	AnyDelegate = FTimerDelegate::CreateLambda([this]()
		{
			NextFunction();
		});

	GetWorld()->GetTimerManager().SetTimer(AnyHandle, AnyDelegate, 3, false);
}

void UISMTutorialOverlay::NextFunction()
{
	if (AISMTutorialGameState* GS = GetWorld()->GetGameState<AISMTutorialGameState>())
	{
		GS->Practice();
	}
}
