// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MapNode.h"
#include "GeodroidGameMode.generated.h"

#define DEBUG true

UCLASS(minimalapi)
class AGeodroidGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:

	AGeodroidGameMode();

	///FPS TEMPLATE CODE MODIFIED FROM HERE ON BELOW
public:

	///MEMBER VARIABLES ********************************************************************************

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Game Design")
		//Array to Set MapNode Walkables.
		//NOTE: negative number implies all Node indices after and at that number shall set to false
		//Eg. (2,-1) => all Nodes from (2,1) to (2,6) shall be set to false
		TArray<FVector2D> MapDesignWalkableArray;

	///MEMBER FUNCTION **********************************************************************************

	///HELPER FUNCTIONS
	UFUNCTION(BlueprintCallable, Category = "Helper Functions")
		static int32 IndexFrom1DTo2D(int32 X, int32 Y, FVector2D ArraySize);

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

	///MEMBER FUNCTIONS *****************************************************************

	UFUNCTION()
		//Function to create the MAP grids for the world
		void CreateMapGrid();

	UFUNCTION()
		//Display the Map in Output log for Debug
		void DisplayMapForDebug();

	UFUNCTION()
		//Function to set the bWalkable of the node from Game Designer provided TArray
		void SetMapWalkables();

	UFUNCTION()
		//Function to extract the bWalkable from the Map
		void MapWalkableExtractor();
};



