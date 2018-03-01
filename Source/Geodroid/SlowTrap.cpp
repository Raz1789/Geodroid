// Fll out your copyright notice in the Description page of Project Settings.

#include "SlowTrap.h"


///***********************************************************************************************************///
///                                               CONSTRUCTOR
///***********************************************************************************************************////
ASlowTrap::ASlowTrap() : Super()
{
	///Setting the Default values to design variables from Parent
	bIsStructureWalkable = true;
	BP_BuildCost = 25;


	///Setting the InfluenceBox
	InfluenceBox = CreateDefaultSubobject<UBoxComponent>(TEXT("InfluenceBox"));
	RootComponent = InfluenceBox;
	float BoxExtends = UMapClass::GetWorldNodeSize() / 2 + /*OFFSET*/ 30.0f; //Setting to the size of a node + Offset
	InfluenceBox->SetBoxExtent(FVector(BoxExtends)); 
	InfluenceBox->bGenerateOverlapEvents = true;
	InfluenceBox->bMultiBodyOverlap = true;
	InfluenceBox->BodyInstance.SetCollisionProfileName("OverlapAllDynamic");

	OnActorBeginOverlap.AddDynamic(this, &ASlowTrap::OnBeginOverlap);
	OnActorEndOverlap.AddDynamic(this, &ASlowTrap::OnEndOverlap);
}

///***********************************************************************************************************///
///                              FUNCTION CALLED WITH ENEMY ENTERS THE COLLISION BOX
///***********************************************************************************************************///
void ASlowTrap::OnBeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	if ((OtherActor != nullptr) && (OtherActor != this))
	{
		ABaseEnemyClass* EnemyActor = Cast<ABaseEnemyClass>(OtherActor);
		if (EnemyActor)
		{
			EnemyActor->AddSpeedEffect();
			UE_LOG(LogTemp, Warning, TEXT("Began!!!"))
		}
	}
}


///***********************************************************************************************************///
///                              FUNCTION CALLED WITH ENEMY EXITS THE COLLISION BOX
///***********************************************************************************************************///
void ASlowTrap::OnEndOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	// Other Actor is the actor that triggered the event. Check that is not ourself.  
	if ((OtherActor != nullptr) && (OtherActor != this))
	{
		ABaseEnemyClass* EnemyActor = Cast<ABaseEnemyClass>(OtherActor);
		if (EnemyActor)
		{
			EnemyActor->ResetSpeedEffect();
			UE_LOG(LogTemp, Warning, TEXT("Ended!!!"))
		}
	}
	
}



