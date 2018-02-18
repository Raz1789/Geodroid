// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MapNode.h"
#include "MapClass.h"
#include "BaseEnemyClass.h"
#include "algorithm"
#include "GeodroidGameMode.generated.h"

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

	///MEMBER VARIABLES
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Design")
		//Array to Set MapNode Walkables.
		//NOTE: negative number implies all Node indices after and at that number shall set to false
		//Eg. (2,-1) => all Nodes from (2,1) to (2,6) shall be set to false
		TArray<FVector2D> MapDesignWalkableArray;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Design")
		//Size of a single node
		int32 NodeWorldSize;
	
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

private:

	///MEMBER VARIABLES

	//stores the number of pawns spawned
	int32 PawnCounter;

	//Variable holding the time from last spawn
	float TimeFromLastSpawn;

	//The Game World Variable
	UWorld* World;

	//Array containing all the pawns
	TArray<ABaseEnemyClass*> EnemyList;
	
	///MEMBER FUNCTIONS

	UFUNCTION()
		//Spawn an Enemy
		void SpawnEnemy(int32 PawnClassIndex);

};



