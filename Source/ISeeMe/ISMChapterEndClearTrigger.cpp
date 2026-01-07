// Fill out your copyright notice in the Description page of Project Settings.


#include "ISMChapterEndClearTrigger.h"

void AISMChapterEndClearTrigger::CompleteChapter()
{
	MulticastPlaySound();
	MulticastShowEpilogue();
}

void AISMChapterEndClearTrigger::MulticastShowEpilogue_Implementation()
{
	OnClearEndUpdated.ExecuteIfBound();
}