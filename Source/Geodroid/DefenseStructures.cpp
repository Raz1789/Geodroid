// Fill out your copyright notice in the Description page of Project Settings.

#include "DefenseStructures.h"

///************** MEMBER FUNCTIONS *************************///

// Sets default values
ADefenseStructures::ADefenseStructures()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	BP_BuildCost = 100;
	bIsStructureActive = false;
}

// Called when the game starts or when spawned
void ADefenseStructures::BeginPlay()
{
	Super::BeginPlay();
	StructureMapNode = UMapClass::WorldToMapNode(GetActorLocation());
	UE_LOG(LogTemp, Warning, TEXT("Node: %s"), *StructureMapNode.NodeIndex.ToString());
}

int32 ADefenseStructures::GetBuildCost()
{
	return BP_BuildCost;
}

FVector2D ADefenseStructures::GetStructureMapNodeIndex()
{
	return StructureMapNode.NodeIndex;
}

TSubclassOf<ADefenseStructures> ADefenseStructures::GetNextLevelStructureActor()
{
	return NextLevelStructureActor;
}

// Called every frame
void ADefenseStructures::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ADefenseStructures::ActivateTower()
{
	bIsStructureActive = true;
}

bool ADefenseStructures::IsStructureActive()
{
	return bIsStructureActive;
}

