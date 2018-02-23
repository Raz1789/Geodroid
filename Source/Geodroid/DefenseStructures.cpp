// Fill out your copyright notice in the Description page of Project Settings.

#include "DefenseStructures.h"
///************* STATIC VARIABLE DECLARATION **************************///
int32 ADefenseStructures::BuildCost;

///************** MEMBER FUNCTIONS *************************///

// Sets default values
ADefenseStructures::ADefenseStructures()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	BP_BuildCost = 100;
}

// Called when the game starts or when spawned
void ADefenseStructures::BeginPlay()
{
	Super::BeginPlay();
	BuildCost = BP_BuildCost;
}

int32 ADefenseStructures::GetBuildCost()
{
	return BuildCost;
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

