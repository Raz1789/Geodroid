// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "MapNode.h"
#include "algorithm"
#include "MapClass.generated.h"

/**
 * 
 */
UCLASS()
class GEODROID_API UMapClass : public UObject
{
	GENERATED_BODY()

public:
	///MEMBER FUNCTION **********************************************************************************

	///HELPER FUNCTIONS
	UFUNCTION(BlueprintCallable, Category = "Helper Functions")
		static void Init(TArray<FVector2D> _MapDesignWalkableArray, float _WorldNodeSize, FVector2D _TargetNode, FVector2D _MapMaxSize);

	UFUNCTION(BlueprintCallable, Category = "Helper Functions")
		static int32 IndexFrom1DTo2D(int32 X, int32 Y, FVector2D ArraySize);

	UFUNCTION(BlueprintCallable, Category = "Helper Functions")
		static FMapNode WorldToMapNode(FVector WorldPosition);

	///GETTERS & SETTERS

	UFUNCTION(BlueprintCallable, Category = "Getter Functions")
		static bool IsDebugOn();

	UFUNCTION(BlueprintCallable, Category = "Setter Functions")
		static void SetMapNodeWalkable(int32 X, int32 Y, bool _bWalkable);

	UFUNCTION(BlueprintCallable, Category = "Getter Functions")
		static FVector GetMapNodePosition(int32 X, int32 Y);

	UFUNCTION(BlueprintCallable, Category = "Getter Functions")
		static bool IsMapNodeWalkable(int32 X, int32 Y);

	UFUNCTION(BlueprintCallable, Category = "Getter Functions")
		static bool IsMapNodeStatusChanged();

	UFUNCTION(BlueprintCallable, Category = "Getter Functions")
		static TArray<bool> GetMapWalkableArray();

	UFUNCTION(BlueprintCallable, Category = "Getter Functions")
		static FVector2D GetMapMaxSize();

	UFUNCTION(BlueprintCallable, Category = "Getter Functions")
		static FMapNode GetTargetNode();

private:
	///MEMBER VARIABLES ****************************************************************

	//Map containing all the nodes in the game world
	static TArray<FMapNode> Map;

	//Returns if the game Map node status is changed
	static bool bMapNodeStatusChanged;

	//Array with all the MapNode bWalkable extracted
	static TArray<bool> MapWalkableArray;

	//Defines the max size of the Map
	static FVector2D MapMaxSize;

	//Array to Set MapNode Walkables.
	//NOTE: negative number implies all Node indices after and at that number shall set to false
	//Eg. (2,-1) => all Nodes from (2,1) to (2,6) shall be set to false
	static TArray<FVector2D> MapDesignWalkableArray;

	//Size of Node in World dimensions
	static float WorldNodeSize;

	//The Target node of the world
	static FMapNode TargetNode;

	///MEMBER FUNCTIONS *****************************************************************

	UFUNCTION()
		//Function to create the MAP grids for the world
		static void CreateMapGrid();

	UFUNCTION()
		//Display the Map in Output log for Debug
		static void DisplayMapForDebug();

	UFUNCTION()
		//Function to set the bWalkable of the node from Game Designer provided TArray
		static void SetMapWalkables();

	UFUNCTION()
		//Function to extract the bWalkable from the Map
		static void MapWalkableExtractor();	
	
};
