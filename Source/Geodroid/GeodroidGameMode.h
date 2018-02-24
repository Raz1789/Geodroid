// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MapNode.h"
#include "MapClass.h"
#include "BaseEnemyClass.h"
#include "algorithm"
#include "GeodroidGameMode.generated.h"

UENUM()
enum class EGameState : uint8
{
	EGS_GameBegan,
	EGS_WaveParamLoading,
	EGS_PlayerWavePrepTime,
	EGS_WaveRunning,
	EGS_WaveEnded,
	EGS_ParamResetting,
	EGS_GameLost,
	EGS_GameEnded
};


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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Design|Level")
		//Array to Set MapNode Walkables.
		//NOTE: negative number implies all Node indices after and at that number shall set to false
		//Eg. (2,-1) => all Nodes from (2,1) to (2,6) shall be set to false
		TArray<FVector2D> MapDesignWalkableArray;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Design|Level")
		//Size of a single node
		int32 NodeWorldSize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Design|Level")
		//Size of Map in # of Nodes
		FVector2D MapMaxSize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Design|Level")
		//Start Node or the Pawn Spawn Location
		FVector2D StartNode;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Design|Level")
		//Target Node
		FVector2D TargetNode;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Design|Enemy")
		//Set the maximum pawns that can be spawned
		int32 MaxSpawnablePawns;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Design|Enemy")
		//Set the maximum time between spawning
		float TimeBetweenSpawn;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Design|Enemy")
		//Start Game Delay
		float TimeBeforeGameStart;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Design|Enemy")
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

	//Variable keeping the time 
	
	///MEMBER FUNCTIONS

	UFUNCTION()
		//Spawn an Enemy
		void SpawnEnemy(int32 PawnClassIndex);

};



