// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// UNREAL HEADER FILES
#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"

// PROJECT HEADER FILES
#include "MapNode.h"

// MANDATE FILES
#include "MapClass.generated.h"

/*****************************************************************************************************************
* CLASS NAME:	UMAPCLASS (STATIC CLASS)
* DESCRIPTION:	The class that encapsulates the Map Information and working.
*				This class has some additional helper functions that can be used outside the function.
*****************************************************************************************************************/
UCLASS()
class GEODROID_API UMapClass : public UObject
{
	GENERATED_BODY()

public:

	///***********************************************************************************************************///
	///                                       PUBLIC MEMBER FUNCTIONS
	///***********************************************************************************************************///

	///----------------------------------- HELPER FUNCTIONS ------------------------------------------------------///
	//Used to initalize this static class variables
	UFUNCTION(BlueprintCallable, Category = "Helper Functions")
	static void Init(TArray<FVector2D> _MapDesignWalkableArray, 
					 float _WorldNodeSize,
					 FVector2D _TargetNode, 
					 FVector2D _MapMaxSize);

	//Helper function to convert 2D index to 1D index
	UFUNCTION(BlueprintCallable, Category = "Helper Functions")
	static int32 IndexFrom2DTo1D(int32 X, int32 Y, FVector2D ArraySize);

	///-------------------------------------- GETTER FUNCTIONS ---------------------------------------------------///

	//Returns the Node position in World Space
	UFUNCTION(BlueprintCallable, Category = "Getter Functions")
	static FVector GetMapNodePosition(int32 X, int32 Y);

	//Convert world position to Map Node
	UFUNCTION(BlueprintCallable, Category = "Helper Functions")
	static FMapNode WorldToMapNode(FVector WorldPosition);

	//Check if a MapNode is Walkable
	UFUNCTION(BlueprintCallable, Category = "Getter Functions")
	static bool IsMapNodeWalkable(int32 X, int32 Y);

	//Check if a structure is build on the node
	UFUNCTION(BlueprintCallable, Category = "Getter Functions")
	static bool IsStructureOnNode(int32 X, int32 Y);

	//Check if MapNode status has changed
	UFUNCTION(BlueprintCallable, Category = "Getter Functions")
	static bool IsMapNodeStatusChanged();

	// retrn the current MapWalkable array
	UFUNCTION(BlueprintCallable, Category = "Getter Functions")
	static TArray<bool> GetMapWalkableArray();

	// returns the Size of the Map in node numbers e.g. 5 Nodes x 5 Nodes
	UFUNCTION(BlueprintCallable, Category = "Getter Functions")
	static FVector2D GetMapMaxSize();

	// returns the Target node in the map where the enemy should reach
	UFUNCTION(BlueprintCallable, Category = "Getter Functions")
	static FMapNode GetTargetNode();
	
	//Get the size of a Node in world Space
	UFUNCTION(BlueprintCallable, Category = "Getter Functions")
	static float GetWorldNodeSize();

	//Resets teh Map node change Status after all the enemy's paths are updated
	UFUNCTION(BlueprintCallable, Category = "Getter Functions")
	static void ResetMapNodeChangeStatus();

	//Check whether the debug is on for the project
	UFUNCTION(BlueprintCallable, Category = "Getter Functions")
	static bool IsDebugOn();

	///-------------------------------------- SETTER FUNCTIONS ---------------------------------------------------///

	//Sets whether a Structure was built on this Node
	UFUNCTION(BlueprintCallable, Category = "Setter Functions")
	static void SetStructureOnNode(int32 X, int32 Y, bool _bStructurePresent);

	//Sets whether the Node is currently walkable or not
	UFUNCTION(BlueprintCallable, Category = "Setter Functions")
	static void SetMapNodeWalkable(int32 X, int32 Y, bool _bWalkable);

private:
	///***********************************************************************************************************///
	///                                       PRIVATE MEMBER VARIABLE
	///***********************************************************************************************************///

	///-------------------------------------- MAP DESIGN VARIABLES ---------------------------------------------------///

	//Defines the max size of the Map
	static FVector2D MapMaxSize;

	//Size of Node in World dimensions
	static float WorldNodeSize;

	//Array to Set MapNode Walkables.
	//NOTE: negative number implies all Node indices after and at that number shall set to false
	//Eg. (2,-1) => all Nodes from (2,1) to (2,6) shall be set to false
	static TArray<FVector2D> MapDesignWalkableArray;

	//The Target node of the world
	static FMapNode TargetNode;

	///-------------------------------------- MAP INFORMATION VARIABLES ---------------------------------------------------///
	
	//Map containing all the nodes in the game world
	static TArray<FMapNode> Map;

	//Array mask containing whether a node has a structure or not
	static TArray<bool> MapStructurePresentArray;

	//Returns if the game Map node status is changed
	static bool bMapNodeStatusChanged;

	//Array with all the MapNode bWalkable extracted
	static TArray<bool> MapWalkableArray;

	///***********************************************************************************************************///
	///                                   PRIVATE MEMBER FUNCTION
	///***********************************************************************************************************////
	///-------------------------------------- CONSTRUCTOR --------------------------------------------------------///
	UMapClass();

	///-------------------------------- CLASS FUNCTIONS ----------------------------------------------------------///
	//Function to create the MAP grids for the world
	UFUNCTION()
	static void CreateMapGrid();

	//Function to set the bWalkable of the node from Game Designer provided TArray
	UFUNCTION()
	static void SetMapWalkables();

	//Function to extract the bWalkable from the Map
	UFUNCTION()
	static void MapWalkableExtractor();

	//Display the Map in Output log for Debug
	UFUNCTION()
	static void DisplayMapForDebug();	
};
