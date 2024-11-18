// Fill out your copyright notice in the Description page of Project Settings.


#include "ISMCrackGround.h"

#include "Components/StaticMeshComponent.h"

#include "ISeeMeCharacter.h"

AISMCrackGround::AISMCrackGround()
{
	PrimaryActorTick.bCanEverTick = true;
	
	GroundMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Ground"));

	RootComponent = GroundMesh;

	BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("Box"));
	BoxCollision->OnComponentBeginOverlap.AddDynamic(this, &AISMCrackGround::OnStep);
	BoxCollision->AttachToComponent(GroundMesh, FAttachmentTransformRules::KeepRelativeTransform);

	CrackGeometry = CreateDefaultSubobject<UGeometryCollectionComponent>(TEXT("CrackGeometry"));
	MulticastCrackAwake(false);
}

void AISMCrackGround::BeginPlay()
{
	Super::BeginPlay();

	if (CrackGeometry)
	{
		CrackGeometry->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
		CrackGeometry->SetRelativeScale3D(FVector(1.0 / GroundMesh->GetComponentScale().X, 1.0 / GroundMesh->GetComponentScale().Y,
			1.0 / GroundMesh->GetComponentScale().Z));
	}

	ResetTimer();
}

void AISMCrackGround::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!bIsCrack)
	{
		RemainTime -= DeltaTime;

		if (RemainTime <= 0.f)
		{
			OnCracked();

			FTimerHandle TimerHandle;
			GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this]() {
				this->ResetTimer();
				}, DormantTime, false);
		}
		else if (RemainTime / CrackTime <= 0.2f && GroundMesh->GetMaterial(0) == HalfCrackMaterial)
		{
			MulticastChangeCrack(MostCrackMaterial);
		}
		else if (RemainTime / CrackTime <= 0.5f && GroundMesh->GetMaterial(0) == FirstCrackMaterial)
		{
			MulticastChangeCrack(HalfCrackMaterial);
		}
	}
}

void AISMCrackGround::OnStep(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!HasAuthority())
		return;

	if (!OtherActor->IsA<AISeeMeCharacter>())
		return;

	UE_LOG(LogTemp, Warning, TEXT("On Step!"));

	SetActorTickEnabled(true);
	MulticastSetCracking(true);
}

void AISMCrackGround::OnCracked()
{
	MulticastCrackAwake(true);
	MulticastSpawnCrackPart();

	bIsCrack = true;
	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this]() {
		this->CrackDestroyTimer();
		}, CrackLifeTime, false);


	SetActorTickEnabled(true);
	MulticastAwake(false);
}

void AISMCrackGround::ResetTimer()
{
	RemainTime = CrackTime;

	SetActorTickEnabled(false);

	MulticastAwake(true);
	MulticastSetCracking(false);

	crackStep = 1;
}

void AISMCrackGround::CrackDestroyTimer()
{
	if (CrackGeometry && GroundMesh)
	{
		TObjectPtr<UGeometryCollection> SavedRestCollection = DuplicateObject<UGeometryCollection>(CrackGeometry->RestCollection, nullptr);

		CrackGeometry->DestroyComponent();
		CrackGeometry = nullptr;

		UGeometryCollectionComponent* NewGeometryCollectionComponent = NewObject<UGeometryCollectionComponent>(this);
		CrackGeometry = NewGeometryCollectionComponent;

		CrackGeometry->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
		CrackGeometry->SetRelativeScale3D(FVector(1.0 / GroundMesh->GetComponentScale().X, 1.0 / GroundMesh->GetComponentScale().Y,
			1.0 / GroundMesh->GetComponentScale().Z));

		if (SavedRestCollection)
		{
			CrackGeometry->SetRestCollection(SavedRestCollection);
		}
		CrackGeometry->RegisterComponent();

		MulticastCrackAwake(false);
		SetActorTickEnabled(false);
		bIsCrack = false;
	}
}

void AISMCrackGround::MulticastAwake_Implementation(bool bInAwake)
{
	if (GroundMesh)
	{
		GroundMesh->SetVisibility(bInAwake);
		GroundMesh->SetCollisionEnabled(bInAwake ? ECollisionEnabled::QueryOnly : ECollisionEnabled::NoCollision);
	}
}

void AISMCrackGround::MulticastCrackAwake_Implementation(bool bInAwake)
{
	if (CrackGeometry)
	{
		CrackGeometry->SetSimulatePhysics(bInAwake);
		CrackGeometry->SetEnableGravity(bInAwake);
		CrackGeometry->SetVisibility(bInAwake);
		CrackGeometry->SetCollisionEnabled(bInAwake ? ECollisionEnabled::QueryAndPhysics : ECollisionEnabled::NoCollision);
	}
}

void AISMCrackGround::MulticastSetCracking_Implementation(bool bInCracking)
{
	if (GroundMesh)
		GroundMesh->SetMaterial(0, bInCracking ? FirstCrackMaterial : BaseMaterial);
}

void AISMCrackGround::MulticastChangeCrack_Implementation(UMaterialInterface* ChangeMaterial)
{
	if (GroundMesh)
	{
		GroundMesh->SetMaterial(0, ChangeMaterial);
	}
}

void AISMCrackGround::MulticastSpawnCrackPart_Implementation()
{
	FVector Location = GetActorLocation() + FVector(0.0f, 0.0f, 59.0f);  

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	FRotator Rotation = FRotator::ZeroRotator;

	CrackPartActor = GetWorld()->SpawnActor<AFieldSystemActor>(CrackPartClass, Location, Rotation, SpawnParams);
	FTimerHandle TimerHandle;

	GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this]() {
		if (CrackPartActor)
		{
			CrackPartActor->Destroy();
		}
		}, 1, false);
}