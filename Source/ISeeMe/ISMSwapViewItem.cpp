// Fill out your copyright notice in the Description page of Project Settings.


#include "ISMSwapViewItem.h"
#include "ISeeMeCharacter.h"
#include "ISeeMeGameMode.h"

// Sets default values
AISMSwapViewItem::AISMSwapViewItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
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

// Called every frame
void AISMSwapViewItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AISMSwapViewItem::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!HasAuthority())
		return;

	MulticastVisibleMesh(false);
	if (AISeeMeGameMode* GM = Cast<AISeeMeGameMode>(GetWorld()->GetAuthGameMode()))
	{
		GetWorldTimerManager().SetTimer(SwapTimerHandle, FTimerDelegate::CreateWeakLambda(this, [GM]()
			{
				GM->SwapCamera();
			}), GM->SwapTime, false);
		GM->SwapCamera();
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
