// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ISMPlayerController.generated.h"

class ACharacter;
/**
 * 
 */
UCLASS()
class ISEEME_API AISMPlayerController : public APlayerController
{
	GENERATED_BODY()

	AISMPlayerController();

protected:
	virtual void OnPossess(APawn* aPawn) override;
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

public:
	void SetOtherCharacter(ACharacter* NewCharacter) { OtherCharacter = NewCharacter; }

	UFUNCTION()
	void OnRep_SwapCamera();

private:
	UPROPERTY(ReplicatedUsing = OnRep_SwapCamera)
	TObjectPtr<class ACharacter> OtherCharacter;
};
