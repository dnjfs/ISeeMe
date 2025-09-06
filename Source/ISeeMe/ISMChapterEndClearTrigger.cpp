// Fill out your copyright notice in the Description page of Project Settings.


#include "ISMChapterEndClearTrigger.h"

void AISMChapterEndClearTrigger::CompleteChapter()
{
	if (OnClearEndUpdated.IsBound())
	{
		OnClearEndUpdated.Execute(true);
	}
	MulticastSaveChapterNo();
	MulticastPlaySound();
}
