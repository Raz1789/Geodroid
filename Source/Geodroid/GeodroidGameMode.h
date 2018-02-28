// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GeodroidCharacter.h"
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

	///MEMBER VARIABLES
	//The player instance
	AGeodroidCharacter* Player;

	//The player controller
	APlayerController* PlayerController;

	//Player HUD
	AGeodroidHUD* PlayerHUD;

	//Check if all pawns have spawned
	bool bAllSpawned;

	//check if this is the first run of the state
	bool bIsThisTheFirstRun;

	//Game Setting to quit
	bool bGameQuitting;

	//Eternal Flame Health
	int32 EternalFlameHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Design|Level")
		//Array to Set MapNode Walkables.
		//NOTE: negative number implies all Node indices after and at that number shall set to false
		//Eg. (2,-1) => all Nodes from (2,1) to (2,6) shall be set to false
		TArray<FVector2D> MapDesignWalkableArray;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Design|Level")
		//Size of a single node
		int32 NodeWorldSize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Design|Level")
		//Size of Map in number of Nodes in X and Y axes
		FVector2D MapMaxSize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Design|Level")
		//Start Node or the Pawn Spawn Location
		FVector2D StartNode;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Design|Level")
		//Target Node
		FVector2D TargetNode;

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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Game Design|Enemy")
	//Current Wave Number
	int32 CurrentWaveNumber;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Design|Enemy")
	//Base Wave Score used to generate the Enemies
	int32 WaveScore;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Game Design|Enemy")
		//Stores the State the game is now
		EGameState CurrentGameState;
	
public:

	virtual void Tick(float DeltaTime) override;

	void StartWave();
	
	UFUNCTION(BlueprintPure, Category = "CPP Functions")
	//Getter for number of Enemies in play
	int32 GetEnemyListLength() const;

	UFUNCTION(BlueprintPure, Category = "CPP Functions")
	//Get Max number of enemies in this Wave
	int32 GetMaxSpawnablePawns() const;

	UFUNCTION(BlueprintPure, Category = "CPP Functions")
	//Get Current Wave Number
	int32 GetCurrentWaveNumber() const;

	UFUNCTION(BlueprintCallable, Category = "CPP Functions")
		//Called when Enemy reaches the Eternal Flame
		void SubtractEternalFlameHealth();

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

	//Clear Dead Enemy from EnemyList
	void ClearDeadEnemy();

};



