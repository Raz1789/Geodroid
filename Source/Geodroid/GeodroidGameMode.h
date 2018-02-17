// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MapNode.h"
#include "BaseEnemyClass.h"
#include "algorithm"
#include "GeodroidGameMode.generated.h"

#define DEBUG true

UCLASS(minimalapi)
class AGeodroidGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:

	AGeodroidGameMode();

	///FPS TEMPLATE CODE MODIFIED FROM HERE ON BELOW
protected:

	virtual void BeginPlay() override;
	
public:

	virtual void Tick(float DeltaTime) override;

	///MEMBER VARIABLES ********************************************************************************

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Design")
		//Array to Set MapNode Walkables.
		//NOTE: negative number implies all Node indices after and at that number shall set to false
		//Eg. (2,-1) => all Nodes from (2,1) to (2,6) shall be set to false
		TArray<FVector2D> MapDesignWalkableArray;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Design")
		//Set the maximum pawns that can be spawned
		int32 MaxSpawnablePawns;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Design")
		//Set the maximum time between spawning
		float TimeBetweenSpawn;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Design")
		//Start Node or the Pawn Spawn Location
		FVector2D StartNode;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Design")
		//Target Node
		FVector2D TargetNode;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Design")
		TArray<TSubclassOf<ABaseEnemyClass>> EnemyClassList;

	///MEMBER FUNCTION **********************************************************************************

	///HELPER FUNCTIONS
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

		//stores the number of pawns spawned
		int32 PawnCounter;

		//Variable holding the time from last spawn
		float TimeFromLastSpawn;

		//The Game World Variable
		UWorld* World;

		//Array containing all the pawns
		TArray<ABaseEnemyClass*> EnemyList;

	///MEMBER FUNCTIONS *****************************************************************

	UFUNCTION()
		//Function to create the MAP grids for the world
		void CreateMapGrid();

	UFUNCTION()
		//Display the Map in Output log for Debug
		static void DisplayMapForDebug();

	UFUNCTION()
		//Function to set the bWalkable of the node from Game Designer provided TArray
		void SetMapWalkables();

	UFUNCTION()
		//Function to extract the bWalkable from the Map
		static void MapWalkableExtractor();

	UFUNCTION()
		//Spawn an Enemy
		void SpawnEnemy(int32 PawnClassIndex);

};



