// Fill out your copyright notice in the Description page of Project Settings.


#include "ISMSwapViewItem.h"
#include "ISeeMeCharacter.h"
#include "ISeeMeGameMode.h"
#include "ISMGameState.h"
#include <Kismet/GameplayStatics.h>
#include "ISeeMe/UI/ISMHUD.h"
#include "ISeeMe/UI/ISMOverlay.h"

// Sets default values
AISMSwapViewItem::AISMSwapViewItem()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CheckPointMesh"));
	if (ItemMesh == nullptr)
		return;

	RootComponent = ItemMesh;

	TriggerVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerVolume"));
	if (TriggerVolume == nullptr)
		return;

	TriggerVolume->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	TriggerVolume->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	TriggerVolume->SetCollisionResponseToChannel(ECC_Pawn, HasAuthority() ? ECR_Overlap : ECR_Ignore);
	TriggerVolume->SetGenerateOverlapEvents(true);

	TriggerVolume->SetupAttachment(ItemMesh);
}

// Called when the game starts or when spawned
void AISMSwapViewItem::BeginPlay()
{
	Super::BeginPlay();

	if (TriggerVolume)
	{
		TriggerVolume->OnComponentBeginOverlap.AddDynamic(this, &AISMSwapViewItem::OnOverlapBegin);
	}
}

void AISMSwapViewItem::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AISeeMeCharacter* OverlappingCharacter = Cast<AISeeMeCharacter>(OtherActor);
	if (HasAuthority() && OverlappingCharacter)
	{
		if (AISMGameState* GS = Cast<AISMGameState>(UGameplayStatics::GetGameState(this)))
		{
			if (GS->SwapViewItem == nullptr)
			{
				GS->SwapViewItem = this;
				MulticastVisibleMesh(false);

				// 아이템 소지 UI 활성화
				MulticastSetOverlayIcon(true);
				MulticastPlaySound();
			}
		}
	}
}

void AISMSwapViewItem::MulticastSetOverlayIcon_Implementation(bool Flag)
{
	if (AISMHUD* ISMHUD = Cast<AISMHUD>(UGameplayStatics::GetPlayerController(this, 0)->GetHUD()))
	{
		if (UISMOverlay* ISMOverlay = ISMHUD->GetISMOverlay())
		{
			ISMOverlay->SetSwapItemIcon(Flag);
		}
	}
}

void AISMSwapViewItem::MulticastVisibleMesh_Implementation(bool bVisible)
{
	ItemMesh->SetVisibility(bVisible);
	if (!bVisible)
	{
		TriggerVolume->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
	else
	{
		TriggerVolume->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		ItemMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	}
}

void AISMSwapViewItem::MulticastPlaySound_Implementation()
{
	UGameplayStatics::PlaySound2D(this, Sound);
}
