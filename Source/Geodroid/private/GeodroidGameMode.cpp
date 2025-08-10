// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "GeodroidGameMode.h"

// UNREAL HEADER FILES
#include "Engine/World.h"

// CPP HEADER FILES
#include "algorithm"

// PROJECT HEADER FILES
#include "BaseEnemyClass.h"
#include "GeodroidHUD.h"
#include "GeodroidCharacter.h"
#include "MapNode.h"
#include "MapClass.h"
#include "PointerProtection.h"

///***********************************************************************************************************///
///                                               CONSTRUCTOR
///***********************************************************************************************************////
AGeodroidGameMode::///***********************************************************************************************************///
	///                                       PUBLIC MEMBER FUNCTIONS
	///***********************************************************************************************************///

	///-------------------------------------- CONSTRUCTOR --------------------------------------------------------///
	AGeodroidGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = AGeodroidHUD::StaticClass();

	///FPS TEMPLATE MODIFIED FROM HERE ON BELOW FOR THIS FUNCTION
	PrimaryActorTick.bCanEverTick = true;

	///Default Initializing for MapWalkableArray

	/// DEFAULT LEVEL DESIGN VARIABLES
	if (MapDesignWalkableArray.Num() == 0)
	{
		MapDesignWalkableArray.Add(FVector2D(0, -1));
		MapDesignWalkableArray.Add(FVector2D(1, -4));
		MapDesignWalkableArray.Add(FVector2D(2, 0));
		MapDesignWalkableArray.Add(FVector2D(2, 1));
		MapDesignWalkableArray.Add(FVector2D(2, -4));
		MapDesignWalkableArray.Add(FVector2D(3, -4));
		MapDesignWalkableArray.Add(FVector2D(4, -4));
		MapDesignWalkableArray.Add(FVector2D(5, 2));
		MapDesignWalkableArray.Add(FVector2D(5, 3));
		MapDesignWalkableArray.Add(FVector2D(5, -4));
		MapDesignWalkableArray.Add(FVector2D(6, -4));
		MapDesignWalkableArray.Add(FVector2D(7, -4));
		MapDesignWalkableArray.Add(FVector2D(8, -4));
		MapDesignWalkableArray.Add(FVector2D(9, 0));
		MapDesignWalkableArray.Add(FVector2D(9, 1));
		MapDesignWalkableArray.Add(FVector2D(9, 2));
		MapDesignWalkableArray.Add(FVector2D(9, -4));
		MapDesignWalkableArray.Add(FVector2D(10, 4));
		MapDesignWalkableArray.Add(FVector2D(10, 6));
		MapDesignWalkableArray.Add(FVector2D(11, 4));
		MapDesignWalkableArray.Add(FVector2D(12, 1));
		MapDesignWalkableArray.Add(FVector2D(12, 2));
		MapDesignWalkableArray.Add(FVector2D(12, 4));
	}

	///Get default game design variables
	NodeWorldSize = 400.f;
	MapMaxSize = FVector2D(15.f, 7.f);
	StartNode = FVector2D(1.f, 0.f);
	TargetNode = FVector2D(10.f, 5.f);

	///WAVE DESIGN VARIABLES
	CurrentWaveNumber = 0;
	WaveScore = 2;

	///ENEMY DESIGN VARIABLES
	TimeBetweenSpawn = 1.f;
	PawnCounter = 0;
	MaxSpawnablePawns = 5;
}

///FPS TEMPLATE CODE MODIFIED FROM HERE ON BELOW

///***********************************************************************************************************///
///                                         BEGIN PLAY FUNCTION
///***********************************************************************************************************////
void AGeodroidGameMode::BeginPlay()
{
	Super::BeginPlay();

	//Creates the Map as per the game design varibles specified in Blueprint or default
	UMapClass::Init(MapDesignWalkableArray, NodeWorldSize, TargetNode, MapMaxSize);

	//Getting the World pointer
	if (UPointerProtection::CheckAndLog(GetWorld(), "World"))
	{
		World = GetWorld();
	}
	
	//Getting the player pointer
	if (World && UPointerProtection::CheckAndLog(World->GetFirstPlayerController(), "PlayerController"))
	{
		if (UPointerProtection::CheckAndLog(World->GetFirstPlayerController()->GetPawn(), "Pawn"))
		{
			if (UPointerProtection::CheckAndLog(Cast<AGeodroidCharacter>(World->GetFirstPlayerController()->GetPawn()), "Casted Pawn"))
			{
				Player = Cast<AGeodroidCharacter>(World->GetFirstPlayerController()->GetPawn());
			}
		}
	}

	//Getting the PlayerController
	if (Player && UPointerProtection::CheckAndLog(Cast<APlayerController>(Player->GetController()),
													TEXT("Player Controller")))
	{
		PlayerController = Cast<APlayerController>(Player->GetController());
	}

	//Getting the PlayerHUD
	if (PlayerController && UPointerProtection::CheckAndLog(Cast<AGeodroidHUD>(PlayerController->GetHUD()),
															TEXT("Player Controller")))
	{
		PlayerHUD = Cast<AGeodroidHUD>(PlayerController->GetHUD());
	}

	//Set the Game State start of the game
	CurrentGameState = EGameState::EGS_GameBegan;


	//set all the default variables (so that it resets every level init)
	bAllSpawned = false;
	bIsThisTheFirstRun = true;
	EternalFlameHealth = 3;
	bGameQuitting = false;
}

///***********************************************************************************************************///
///                                         TICK FUNCTION
///***********************************************************************************************************////
void AGeodroidGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	///Pointer Protection
	if (!World) return;
	if (!Player) return;
	if (!PlayerHUD) return;

	///Clear Dead Enemies from the EnemyList
	ClearDeadEnemy();

	///Retrieving information from Player to start wave
	if (CurrentGameState != EGameState::EGS_PlayerWavePrepTime && Player->ShouldWaveStart())
	{
		Player->ResetStartWave();
		Player->ResetIsGameModeReady();
	}

	///Retrieving information from Player about the health
	if (Player->GetPlayerHealth() <= 0 && !bGameQuitting)
	{
		CurrentGameState = EGameState::EGS_GameEnded;
		bIsThisTheFirstRun = true;
		bGameQuitting = true;
	}

	///check if the flame health is above 0
	if (EternalFlameHealth <= 0 && !bGameQuitting)
	{
		CurrentGameState = EGameState::EGS_GameEnded;
		bIsThisTheFirstRun = true;
		bGameQuitting = true;
	}

	///Main Game state logic
	switch (CurrentGameState)
	{
	case EGameState::EGS_GameBegan:

		CurrentGameState = EGameState::EGS_WaveParamLoading;

		if (!bIsThisTheFirstRun)	bIsThisTheFirstRun = true;
		break;
	case EGameState::EGS_WaveParamLoading:
		
		WaveParamLoading();

		if(!bIsThisTheFirstRun)	bIsThisTheFirstRun = true;
		break;
	case EGameState::EGS_PlayerWavePrepTime:

		PlayerWavePreptime();

		break;
	case EGameState::EGS_WaveRunning:

		WaveRunning(DeltaTime);

		break;
	case EGameState::EGS_WaveEnded:
		if (bIsThisTheFirstRun)
		{
			PlayerHUD->ReceivePopUpMessage("Wave Ended");
		}

		//Check if to continue the game
		if (Player->GetPlayerHealth() > 0)
		{
			CurrentGameState = EGameState::EGS_WaveParamLoading;
		}
		else
		{
			CurrentGameState = EGameState::EGS_GameEnded;
		}

		bIsThisTheFirstRun = true;
		break;
	case EGameState::EGS_GameEnded:
		bIsThisTheFirstRun = true;
		break;
	}
}

///***********************************************************************************************************///
///                                         WAVE RUNNING LOGIC
///***********************************************************************************************************////
void AGeodroidGameMode::WaveRunning(float DeltaTime)
{
	if (bIsThisTheFirstRun)
	{
		PlayerHUD->ReceivePopUpMessage("Enemies Spawning..... Buck up!!!");
		bIsThisTheFirstRun = false;
	}

	//Update the SpawnTimer
	if (PawnCounter < MaxSpawnablePawns)
	{
		TimeFromLastSpawn += DeltaTime;
	}

	//Check if the SpawnDelay has crossed && if PawnCounter is less than the MaxSpawnablePawns
	if ((TimeFromLastSpawn > TimeBetweenSpawn) && PawnCounter < MaxSpawnablePawns)
	{
		//Reset CurrentTimer
		TimeFromLastSpawn = 0.f;
		//Spawn an Enemy
		SpawnEnemy(0);
		//Increment PawnCounter
		PawnCounter++;
	}

	//check if all pawns spawned
	if (PawnCounter >= MaxSpawnablePawns)
	{
		bAllSpawned = true;
	}

	//Update the Enemy PathList if the Map has Node has changed Status
	UpdateEnemyPathList();

	//Check if EnemyReached Endpoint
	CheckEnemyReachedEndpoint();

	//Check if eligible to go to next phase Next condition
	if (EnemyList.Num() < 1 && bAllSpawned)
	{
		bAllSpawned = false;
		CurrentGameState = EGameState::EGS_WaveEnded;
		bIsThisTheFirstRun = true;
	}
}

///***********************************************************************************************************///
///                             UPDATE PATHLIST IF MAP NODE STATUS CHANGES
///***********************************************************************************************************////
void AGeodroidGameMode::UpdateEnemyPathList()
{
	if (UMapClass::IsMapNodeStatusChanged())
	{
		if (EnemyList.Num() > 0)
		{
			for (auto& Enemy : EnemyList)
			{
				Enemy->UpdatePathList();
			}
		}
		UMapClass::ResetMapNodeChangeStatus();
	}
}

///***********************************************************************************************************///
///                             CHECK ENEMY REACHED THE ENDPOINT
///***********************************************************************************************************////
void AGeodroidGameMode::CheckEnemyReachedEndpoint()
{
	if (EnemyList.Num() > 0)
	{
		for (auto& Enemy : EnemyList)
		{
			if (Enemy)
			{
				bool bHasEnemyReachedEndpoint = Enemy->HasEnemyReachedEndpoint();
				if (bHasEnemyReachedEndpoint)
				{
					SubtractEternalFlameHealth();
					Enemy->ResetReachedEndPointSuccessfully();
				}
			}
		}
	}
}

///***********************************************************************************************************///
///                             PREP PHASE UPDATES
///***********************************************************************************************************////
void AGeodroidGameMode::PlayerWavePreptime()
{
	if (bIsThisTheFirstRun)
	{
		PlayerHUD->ReceivePopUpMessage("Press \'ENTER\' to start the wave");
		bIsThisTheFirstRun = false;
	}

	//Check if player has started the wave
	if (Player->ShouldWaveStart())
	{
		CurrentGameState = EGameState::EGS_WaveRunning;

		Player->ResetStartWave();
		Player->ResetIsGameModeReady();

		if (!bIsThisTheFirstRun)	
			bIsThisTheFirstRun = true;
	}
	else
	{
		Player->SetIsGameModeReady(); //Tells player that GameMode is ready to receive the StartWave input
	}
}

///***********************************************************************************************************///
///                             WAVE PARAMETER SETTING
///***********************************************************************************************************////
void AGeodroidGameMode::WaveParamLoading()
{
	if (bIsThisTheFirstRun)
	{
		CurrentWaveNumber++;
		WaveScore += CurrentWaveNumber;
		MaxSpawnablePawns = WaveScore;
		PawnCounter = 0; //Resetting PawnCounter
		TimeFromLastSpawn = 0; //Resetting the Timer
	}
	CurrentGameState = EGameState::EGS_PlayerWavePrepTime;
}

///***********************************************************************************************************///
///                             START WAVE UPDATES
///***********************************************************************************************************////
void AGeodroidGameMode::StartWave()
{
	if (CurrentGameState == EGameState::EGS_PlayerWavePrepTime)
	{
		CurrentGameState = EGameState::EGS_WaveRunning;
	}
}

///***********************************************************************************************************///
///                       GET THE NUMBER OF ENEMIES ON THE GAME
///***********************************************************************************************************////
int32 AGeodroidGameMode::GetEnemyListLength() const
{
	return EnemyList.Num();
}

///***********************************************************************************************************///
///                    GET THE MAX ENEMIES THAT WILL BE SPAWNED IN THIS WAVE
///***********************************************************************************************************////
int32 AGeodroidGameMode::GetMaxSpawnablePawns() const
{
	return MaxSpawnablePawns;
}

///***********************************************************************************************************///
///                             GET THE CURRENT WAVE NUMBER
///***********************************************************************************************************////
int32 AGeodroidGameMode::GetCurrentWaveNumber() const
{
	return CurrentWaveNumber;
}

///***********************************************************************************************************///
///                             APPLY DAMAGE TO THE FLAME HEALTH
///***********************************************************************************************************////
void AGeodroidGameMode::SubtractEternalFlameHealth()
{
	EternalFlameHealth--;
}

///***********************************************************************************************************///
///                             SPAWN THE ENEMY
///***********************************************************************************************************////
void AGeodroidGameMode::SpawnEnemy(int32 PawnClassIndex)
{
	//Pointer Protections
	if (!World) return;

	if (EnemyClassList.Num() != 0 && EnemyClassList[PawnClassIndex]) //EnemyClassList check is second since it prevent from crashing
	{
			FVector position = FVector(StartNode.X, StartNode.Y, 100.f);

			//Spawn the actor at that location
			FActorSpawnParameters SpawnParams;
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
			EnemyList.Add(World->SpawnActor<ABaseEnemyClass>(EnemyClassList[PawnClassIndex], position, FRotator(0.f), SpawnParams));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("EnemyClass Class not initialized"));

	}
}

///***********************************************************************************************************///
///                             CLEAR THE DEAD ENEMIES FROM THE ENEMY LIST
///***********************************************************************************************************////
void AGeodroidGameMode::ClearDeadEnemy()
{
	for (int32 EnemyCounter = EnemyList.Num() - 1; EnemyCounter >= 0; --EnemyCounter)
	{
		if (::IsValid(EnemyList[EnemyCounter]))
		{
			EnemyList.RemoveAt(EnemyCounter);
		}
	}
}
