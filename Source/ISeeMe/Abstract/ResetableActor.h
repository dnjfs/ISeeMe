// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ResetableActor.generated.h"

UCLASS(Abstract)
class ISEEME_API AResetableActor : public AActor
{
	GENERATED_BODY()

public:
	AResetableActor();

protected:
	virtual void PostInitializeComponents() override;

	UFUNCTION()
	virtual void OnReset() PURE_VIRTUAL(AResetableActor::OnResetEvent);
};
