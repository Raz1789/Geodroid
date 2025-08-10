// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "UObject/ConstructorHelpers.h"
#include "GeodroidGameMode.generated.h"

// ENUM GAME STATE
UENUM()
enum class EGameState : uint8
{
	EGS_GameBegan,
	EGS_WaveParamLoading,
	EGS_PlayerWavePrepTime,
	EGS_WaveRunning,
	EGS_WaveEnded,
	EGS_GameEnded
};

/*****************************************************************************************************************
* CLASS NAME:	AGEODROIDGAMEMODE
* DESCRIPTION:	THE GAME MANAGER
*****************************************************************************************************************/

UCLASS(minimalapi)
class AGeodroidGameMode : public AGameModeBase
{
	GENERATED_BODY()

	///FPS TEMPLATE CODE MODIFIED FROM HERE ON BELOW
protected:
	///***********************************************************************************************************///
	///                                       PROTECTED MEMBER VARIABLE
	///***********************************************************************************************************///

	///-------------------------------------- DESIGN VARIABLES ---------------------------------------------------///
	//Eternal Flame Health
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Game Design|Flame")
	int32 EternalFlameHealth;

	//Array to Set MapNode Walkables.
	//NOTE: negative number implies all Node indices after and at that number shall set to false
	//Eg. (2,-1) => all Nodes from (2,1) to (2,6) shall be set to false
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Design|Level")
	TArray<FVector2D> MapDesignWalkableArray;

	//Size of a single node
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Design|Level")
	int32 NodeWorldSize;

	//Size of Map in number of Nodes in X and Y axes
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Design|Level")
	FVector2D MapMaxSize;

	//Start Node or the Pawn Spawn Location
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Design|Level")
	FVector2D StartNode;

	//Target Node
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Design|Level")
	FVector2D TargetNode;

	//Set the maximum time between spawning
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Design|Enemy")
	float TimeBetweenSpawn;

	//Start Game Delay
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Design|Enemy")
	float TimeBeforeGameStart;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Design|Enemy")
	TArray<TSubclassOf<class ABaseEnemyClass>> EnemyClassList;

	//Current Wave Number
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Game Design|Enemy")
	int32 CurrentWaveNumber;

	//Base Wave Score used to generate the Enemies
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Design|Enemy")
	int32 WaveScore;

	///-------------------------------------- GAME RUN VARIABLES ---------------------------------------------------///
	//Check if all pawns have spawned
	bool bAllSpawned;

	//check if this is the first run of the state
	bool bIsThisTheFirstRun;

	//Game Setting to quit
	bool bGameQuitting;

	//Set the maximum pawns that can be spawned
	int32 MaxSpawnablePawns;
	///***********************************************************************************************************///
	///                                       PROTECTED MEMBER FUNCTIONS
	///***********************************************************************************************************///

	///-------------------------------------- OVERRIDE FUNCTIONS -----------------------------------------------///
	virtual void BeginPlay() override;
	
public:

	///***********************************************************************************************************///
	///                                       PUBLIC MEMBER VARIABLE
	///***********************************************************************************************************///

	//Stores the State the game is now
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Design|Enemy")
	EGameState CurrentGameState;

	///***********************************************************************************************************///
	///                                       PUBLIC MEMBER FUNCTIONS
	///***********************************************************************************************************///

	///-------------------------------------- CONSTRUCTOR --------------------------------------------------------///
	AGeodroidGameMode();

	///-------------------------------------- OVERRIDE FUNCTION --------------------------------------------------///
	virtual void Tick(float DeltaTime) override;
	
	///-------------------------------------- GETTER FUNCTION --------------------------------------------------///

	//Getter for number of Enemies in play
	UFUNCTION(BlueprintPure, Category = "CPP Functions")
	int32 GetEnemyListLength() const;

	//Get Max number of enemies in this Wave
	UFUNCTION(BlueprintPure, Category = "CPP Functions")
	int32 GetMaxSpawnablePawns() const;

	//Get Current Wave Number
	UFUNCTION(BlueprintPure, Category = "CPP Functions")
	int32 GetCurrentWaveNumber() const;

	//Called when Enemy reaches the Eternal Flame
	UFUNCTION(BlueprintCallable, Category = "CPP Functions")
	void SubtractEternalFlameHealth();

private:
	///***********************************************************************************************************///
	///                                       PRIVATE MEMBER VARIABLES
	///***********************************************************************************************************///

	///-------------------------------------- COMMON VARIABLES ---------------------------------------------------///
	//The player instance
	class AGeodroidCharacter* Player;

	//The player controller
	class APlayerController* PlayerController;

	//Player HUD
	class AGeodroidHUD* PlayerHUD;

	//stores the number of pawns spawned
	int32 PawnCounter;

	//Variable holding the time from last spawn
	float TimeFromLastSpawn;

	//The Game World Variable
	class UWorld* World;

	//Array containing all the pawns
	TArray<ABaseEnemyClass*> EnemyList;
	
	///***********************************************************************************************************///
	///                                       PRIVATE MEMBER FUNCTIONS
	///***********************************************************************************************************///
	
	///-------------------------------------- HELPER FUNCTION ----------------------------------------------------///
	UFUNCTION()
	//Spawn an Enemy
	void SpawnEnemy(int32 PawnClassIndex);

	//Clear Dead Enemy from EnemyList
	void ClearDeadEnemy();

	///------------------------------------ GAME STATE FUNCTION --------------------------------------------------///
	void WaveRunning(float DeltaTime);

	void UpdateEnemyPathList();

	void CheckEnemyReachedEndpoint();

	void PlayerWavePreptime();

	void WaveParamLoading();

	void StartWave();

};



