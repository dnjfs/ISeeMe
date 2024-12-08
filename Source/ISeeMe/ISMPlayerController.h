// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ISeeMeCharacter.h"
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
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* aPawn) override;
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

public:
	void SetOtherCharacter(AISeeMeCharacter* NewCharacter) { OtherCharacter = NewCharacter; }
	AISeeMeCharacter* GetOtherCharacter() { return OtherCharacter; }

	UFUNCTION()
	void OnRep_SwapCamera();

	/** Swap camera for Game Mode */
	UFUNCTION()
	void SwapCamera(bool bItem);

	/** Client call for Server swap camera*/
	UFUNCTION(Server, Reliable)
	void ServerCallSwapCamera();

	/** Called ChangeUnHideBone() for server, client */
	UFUNCTION(NetMulticast, Reliable)
	void RecoverAspect();

	/** Swap aspect */
	UFUNCTION()
	void SwapAspect();

	/** Stay current aspect */
	UFUNCTION()
	void CurrentAspect();

	/** To unhide all bones */
	UFUNCTION()
	void ChangeUnHideBone(AISeeMeCharacter* OtherCharacter);

	UPROPERTY(ReplicatedUsing = OnRep_SwapCamera)
	TObjectPtr<class AISeeMeCharacter> OtherCharacter;

private:
	bool bFirstAspect = false;

	/** Update Aspect */
	void UpdateAspect();

	/** Change First Aspect */
	void ChangeFirstAspect();

	/** Change Third Aspect */
	void ChangeThirdAspect();
};
