// Fill out your copyright notice in the Description page of Project Settings.

#include "DefenseStructures.h"

// PROJECT HEADER FILES
#include "MapClass.h"

///***********************************************************************************************************/// 
///                                       CONSTRUCTORS
///***********************************************************************************************************///
ADefenseStructures::ADefenseStructures()
{
	BP_BuildCost = 100;
	bIsStructureActive = false;
}

///***********************************************************************************************************///
///                                       BEGIN PLAY FUNCTION
///***********************************************************************************************************///
void ADefenseStructures::BeginPlay()
{
	Super::BeginPlay();
	StructureMapNode = UMapClass::WorldToMapNode(GetActorLocation());
}

///***********************************************************************************************************///
///                                       GETTTERS
///***********************************************************************************************************///

///------------------------------- GET THE BUILDING COST -----------------------------------------------------///
int32 ADefenseStructures::GetBuildCost()
{
	return BP_BuildCost;
}

///--------------------------- GET THE STRUCTURE'S NODE INDEX FROM MAP ---------------------------------------///
FVector2D ADefenseStructures::GetStructureMapNodeIndex()
{
	return StructureMapNode.NodeIndex;
}

///-------------------------- GET IF THE STRUCTURE HAS BEEN ACTIVATED ----------------------------------------///
bool ADefenseStructures::IsStructureActive()
{
	return bIsStructureActive;
}

///----------------------------- GET IF THE STRUCTURE IS WALKABLE --------------------------------------------///
bool ADefenseStructures::IsStructureWalkable()
{
	return bIsStructureWalkable;
}

///***********************************************************************************************************///
///                                       SETTTERS
///***********************************************************************************************************///

///---------------------------------SETTER TO ACTIVATE THE TOWER ---------------------------------------------///
void ADefenseStructures::ActivateTower()
{
	bIsStructureActive = true;
}

