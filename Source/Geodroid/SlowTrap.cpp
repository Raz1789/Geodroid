// Fll out your copyright notice in the Description page of Project Settings.

#include "SlowTrap.h"

ASlowTrap::ASlowTrap() : Super()
{
	bIsStructureWalkable = true;

	BP_BuildCost = 25;

	///Setting the InfluenceBox
	InfluenceBox = CreateDefaultSubobject<UBoxComponent>(TEXT("InfluenceBox"));
	RootComponent = InfluenceBox;
	float BoxExtends = UMapClass::GetWorldNodeSize() / 2;
	InfluenceBox->SetBoxExtent(FVector(BoxExtends));
	InfluenceBox->bGenerateOverlapEvents = true;
	InfluenceBox->bMultiBodyOverlap = true;
	InfluenceBox->BodyInstance.SetCollisionProfileName("OverlapAllDynamic");
}

void ASlowTrap::BeginPlay()
{
	OnActorBeginOverlap.AddDynamic(this, &ASlowTrap::OnBeginOverlap);
	OnActorEndOverlap.AddDynamic(this, &ASlowTrap::OnEndOverlap);
}

void ASlowTrap::OnBeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	if ((OtherActor != nullptr) && (OtherActor != this))
	{
		ABaseEnemyClass* EnemyActor = Cast<ABaseEnemyClass>(OtherActor);
		if (EnemyActor)
		{
			EnemyActor->AddSpeedEffect();
		}
	}
}

void ASlowTrap::OnEndOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	// Other Actor is the actor that triggered the event. Check that is not ourself.  
	if ((OtherActor != nullptr) && (OtherActor != this))
	{
		ABaseEnemyClass* EnemyActor = Cast<ABaseEnemyClass>(OtherActor);
		if (EnemyActor)
		{
			EnemyActor->ResetSpeedEffect();
		}
	}
	
}



