// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ISMAngel.generated.h"

UCLASS()
class ISEEME_API AISMAngel : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AISMAngel();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	void Move(bool bInformation);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Location")
	TArray<FVector> InformationLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Location")
	TArray<FVector> PracticeLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Location")
	TArray<TSubclassOf<AActor>> TutorialItems;

	UPROPERTY(EditAnywhere)
	USceneComponent* SpawnPoint;

private:
	TArray<bool> bTutorialSpawn;

};
