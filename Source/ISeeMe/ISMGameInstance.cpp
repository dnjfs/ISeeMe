// Fill out your copyright notice in the Description page of Project Settings.


#include "ISMGameInstance.h"

void UISMGameInstance::SetSelectedPawnClass(TSubclassOf<APawn> NewPawnClass)
{
	SelectedPawnClass = NewPawnClass;
}