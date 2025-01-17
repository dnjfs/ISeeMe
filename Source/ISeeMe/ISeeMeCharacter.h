// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "OnlineSubsystemUtils.h"
#include "ISeeMeCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;
class UTimelineComponent;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS(config=Game)
class AISeeMeCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AISeeMeCharacter();

	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	/** When first perspective, hide bone name */
	UPROPERTY(EditAnywhere, Category = "Bone")
	FName HideBoneName;

	/**  How high to dead */
	UPROPERTY(EditAnywhere)
	float DeadHeight=1000;

	UPROPERTY(ReplicatedUsing = OnRep_IsCameraRestored)
	bool IsCameraRestored = true;

	/** Call Go Check Point Function from client to the server **/
	UFUNCTION(Server, Reliable)
	void ServerCallGoCheckPoint();

	/** Called for swap camera */
	void SwapCamera();

	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void CallSelectPawn(TSubclassOf<APawn> NewPawn);

	UFUNCTION()
	void SelectPawn(TSubclassOf<APawn> NewPawn, int num);

	UFUNCTION(Server, Reliable)
	void ServerSelectPawn(TSubclassOf<APawn> NewPawn);

protected:
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	/** Called for focus input */
	void Focus();

	/** Called for enable voice */
	void EnableVoice();

	/** Called for disable voice */
	void DisableVoice();

	/** Called for swap aspect */
	void SwapAspect();

	/** Called for open menu */
	void OpenMenu();

	/** Called for go check point */
	void GoCheckPoint();

	/** Called for go check point */
	void CallGoCheckPoint();

	/** Called for control pitch */
	void ControlPitch(const FInputActionValue& Value);

	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// To add mapping context
	virtual void BeginPlay();

	bool InitVoiceChat();

private:
	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;
	
	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;

	/** Focus Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* FocusAction;

	/** Toggle Voice Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* ToggleVoiceAction;

	/** Swap Camera Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* SwapCameraAction;

	/** Swap Aspect Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* SwapAspectAction;

	/** Open Menu Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* OpenMenuAction;

	/** Go Check Point Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* GoCheckPointAction;

	/** Go Check Point Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* ControlPitchAction;

	/* Voice Chat */
	IOnlineVoicePtr OnlineVoicePtr;

	/* Focus */
	UPROPERTY(EditDefaultsOnly, Category = Timeline, meta = (AllowPrivateAccess = "true"))
	UTimelineComponent* FocusTimeline;

	UPROPERTY(EditDefaultsOnly, Category = Timeline, meta = (AllowPrivateAccess = "true"))
	UCurveFloat* FocusCurve;

	FRotator FocusStartRotator;
	FRotator FocusEndRotator;

	/*Judgement falling */
	bool bIsFalling = true;

	/* When fall, first location */
	float FirstHeight = 0;

	UFUNCTION()
	void PlayFocusTimeline(float Value);

	UFUNCTION()
	void OnRep_IsCameraRestored();
};

