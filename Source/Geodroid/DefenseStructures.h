// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// UNREAL HEADER FILES
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SphereComponent.h"

// PROJECT HEADER FILES
#include "MapClass.h"
#include "MapNode.h"

// MANDATE FILES
#include "DefenseStructures.generated.h"

UCLASS()
class GEODROID_API ADefenseStructures : public AActor
{
	GENERATED_BODY()

protected:
	///***********************************************************************************************************///
	///                                       PROTECTED MEMBER VARIABLE
	///***********************************************************************************************************///
	
	///-------------------------------------- STRUCTURE DESIGN VARIABLES -----------------------------------------///
	UPROPERTY(EditDefaultsOnly, Category = "Structure Design")
		//Specifies if the Node on which the Structure exist is walkable or not
		bool bIsStructureWalkable;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Structure Design")
		//Specifies the Attack Rate in Seconds
		float AttackRate;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Structure Design")
		//Attack Damage per shot by the Structure
		float AttackDamage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Structure Design")
		//Get the Building Cost from the Blueprint
		int32 BP_BuildCost;

	///-------------------------------------- COMMON VARIABLES ---------------------------------------------------///
	//The Structure's Map Node Index
	FMapNode StructureMapNode;

	//Check if the DefenseStructure is Activated
	bool bIsStructureActive;


	///***********************************************************************************************************///
	///                                       PROTECTED MEMBER FUNCTIONS
	///***********************************************************************************************************///

	///-------------------------------------- OVERRIDE FUNCTIONS -----------------------------------------------///
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:

	///***********************************************************************************************************///
	///                                       PUBLIC MEMBER FUNCTIONS
	///***********************************************************************************************************///

	///-------------------------------------- CONSTRUCTOR --------------------------------------------------------///
	// Sets default values for this actor's properties
	ADefenseStructures();

	///-------------------------------------- CLASS FUNCTIONS -----------------------------------------------///
	//Activates the Tower
	void ActivateTower();

	//Getter for bIsStructureActive
	bool IsStructureActive();

	//Getter for bIsStructureWalkable
	bool IsStructureWalkable();

	//Getter for BUILD_COST
	int32 GetBuildCost();

	//Getter for Structure Map Node Index
	FVector2D GetStructureMapNodeIndex();

};
