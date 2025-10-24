// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "ISMCharacterMovementComponent.generated.h"

/**
 * 
 */
UCLASS()
class ISEEME_API UISMCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()
	
public:
	UISMCharacterMovementComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
};
