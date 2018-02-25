// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
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
	bool bIsNodeWalkable;

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

	//Cost of Constructing the Defense Structure (Constant for a Defense Structure)
	static int32 BuildCost;

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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Design")
	//Get the Building Cost from the Blueprint
	int32 BP_BuildCost;

	///************************** MEMBER FUNCTION **********************************///

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//Activates the Tower
	void ActivateTower();

	//Getter for bIsStructureActive
	bool IsStructureActive();

	//Getter for BUILD_COST
	static int32 GetBuildCost();

	//Getter for NextLevelStructureActor
	TSubclassOf<ADefenseStructures> GetNextLevelStructureActor();	
	
};
