// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MapClass.h"
#include "MapNode.h"
#include "Components/SphereComponent.h"
#include "DefenseStructures.generated.h"

UENUM()
enum class ELevel : uint8
{
	Level1,
	Level2,
	Level3
};

UCLASS()
class GEODROID_API ADefenseStructures : public AActor
{
	GENERATED_BODY()
	
public:	
	///******** CONSTRUCTOR *******************///

	// Sets default values for this actor's properties
	ADefenseStructures();

protected:
	///**************************** MEMBER VARIABLES **********************************///
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Design")
	//Specifies if the Node on which the Structure exist is walkable or not
	bool bIsStructureWalkable; //TODO use this variable for checking

	//The Structure's Map Node Index
	FMapNode StructureMapNode;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Design")
	//Specifies the Attack Rate in Seconds
	float AttackRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Design")
	//Attack Damage per shot by the Structure
	float AttackDamage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Design")
	//Current level of the DefenseStructure
	ELevel CurrentLevelOfStructure;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Design")
	//Limit to the upgrade of this structure
	ELevel BuildLevelLimit;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Design")
		//Get the Building Cost from the Blueprint
		int32 BP_BuildCost;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Design")
	//Stores the next Level Blueprint
	TSubclassOf<ADefenseStructures> NextLevelStructureActor;

	//Check if the DefenseStructure is Activated
	bool bIsStructureActive;

	
	///**************************** MEMBER FUNCTIONS **********************************///
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:

	///*************************** MEMBER VARIABLE **********************************///
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Item Design")
		//Color of Material
		FColor MaterialColor;

	///************************** MEMBER FUNCTION **********************************///

	// Called every frame
	virtual void Tick(float DeltaTime) override;

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

	//Getter for NextLevelStructureActor
	TSubclassOf<ADefenseStructures> GetNextLevelStructureActor();	
	
};
