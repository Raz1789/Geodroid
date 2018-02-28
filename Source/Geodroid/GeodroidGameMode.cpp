// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "GeodroidGameMode.h"
#include "GeodroidHUD.h"
#include "GeodroidCharacter.h"
#include "UObject/ConstructorHelpers.h"

/// MEMBER FUNCTIONS
AGeodroidGameMode::AGeodroidGameMode()
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

	/// LEVEL DESIGN VARIABLES
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

void AGeodroidGameMode::BeginPlay()
{
	Super::BeginPlay();

	TimeFromLastSpawn = TimeBetweenSpawn; //To spawn immediately;

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
	if (Player && UPointerProtection::CheckAndLog(Cast<APlayerController>(Player->GetController()), TEXT("Player Controller")))
	{
		PlayerController = Cast<APlayerController>(Player->GetController());
	}

	//Getting the PlayerHUD
	if (PlayerController && UPointerProtection::CheckAndLog(Cast<AGeodroidHUD>(PlayerController->GetHUD()), TEXT("Player Controller")))
	{
		PlayerHUD = Cast<AGeodroidHUD>(PlayerController->GetHUD());
	}

	CurrentGameState = EGameState::EGS_GameBegan;

	bAllSpawned = false;
	bIsThisTheFirstRun = true;
	
}

void AGeodroidGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!Player) return;
	if (!PlayerHUD) return;

	ClearDeadEnemy();

	if (CurrentGameState != EGameState::EGS_PlayerWavePrepTime && Player->ShouldWaveStart())
	{
		Player->ResetStartWave();
		Player->ResetIsGameModeReady();
	}

	switch (CurrentGameState)
	{
	case EGameState::EGS_GameBegan:

		CurrentGameState = EGameState::EGS_WaveParamLoading;

		if (!bIsThisTheFirstRun)	bIsThisTheFirstRun = true;
		break;
	case EGameState::EGS_WaveParamLoading:
		if (bIsThisTheFirstRun)
		{
			CurrentWaveNumber++;
			WaveScore += CurrentWaveNumber;
			MaxSpawnablePawns = WaveScore;
			PawnCounter = 0; //Resetting PawnCounter
			TimeFromLastSpawn = 0; //Resetting the Timer
		}
		CurrentGameState = EGameState::EGS_PlayerWavePrepTime;

		if(!bIsThisTheFirstRun)	bIsThisTheFirstRun = true;
		break;
	case EGameState::EGS_PlayerWavePrepTime:
		if (bIsThisTheFirstRun)
		{
			PlayerHUD->ReceivePopUpMessage("Press \'ENTER\' to start the wave");
			bIsThisTheFirstRun = false;
		}
		if (Player->ShouldWaveStart())
		{
			CurrentGameState = EGameState::EGS_WaveRunning;
			Player->ResetStartWave();
			Player->ResetIsGameModeReady();
		}
		else
		{
			Player->SetIsGameModeReady();
		}
		break;
	case EGameState::EGS_WaveRunning:

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

		//Check if eligible to go to next phase Next condition
		if (EnemyList.Num() < 1 && bAllSpawned)
		{
			bAllSpawned = false;
			CurrentGameState = EGameState::EGS_WaveEnded;
		}

		if (!bIsThisTheFirstRun)	bIsThisTheFirstRun = true;
		break;
	case EGameState::EGS_WaveEnded:
		CurrentGameState = EGameState::EGS_ParamResetting;
		if (!bIsThisTheFirstRun)	bIsThisTheFirstRun = true;
		break;
	case EGameState::EGS_ParamResetting:
		CurrentGameState = EGameState::EGS_WaveParamLoading;
		if (!bIsThisTheFirstRun)	bIsThisTheFirstRun = true;
		break;
	case EGameState::EGS_GameLost:
		if (!bIsThisTheFirstRun)	bIsThisTheFirstRun = true;
		break;
	case EGameState::EGS_GameEnded:
		if (!bIsThisTheFirstRun)	bIsThisTheFirstRun = true;
		break;
	}
}

void AGeodroidGameMode::StartWave()
{
	if (CurrentGameState == EGameState::EGS_PlayerWavePrepTime)
	{
		CurrentGameState = EGameState::EGS_WaveRunning;
	}
}

int32 AGeodroidGameMode::GetEnemyListLength() const
{
	return EnemyList.Num() - 1;
}

int32 AGeodroidGameMode::GetMaxSpawnablePawns() const
{
	return MaxSpawnablePawns;
}

int32 AGeodroidGameMode::GetCurrentWaveNumber() const
{
	return CurrentWaveNumber;
}

void AGeodroidGameMode::SpawnEnemy(int32 PawnClassIndex)
{
	if (!World) return;
	if (EnemyClassList.Num() != 0 && EnemyClassList[PawnClassIndex]) //EnemyClassList second since it prevent from crashing
	{
			FVector position = FVector(StartNode.X, StartNode.Y, 100.f); //TODO use the Map to get the Start Node

			//Spawn the actor at that location
			FActorSpawnParameters SpawnParams;
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;
			EnemyList.Add(World->SpawnActor<ABaseEnemyClass>(EnemyClassList[PawnClassIndex], position, FRotator(0.f), SpawnParams));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("EnemyClass Class not initialized"));

	}
}

void AGeodroidGameMode::ClearDeadEnemy()
{
	for (int32 EnemyCounter = 0; EnemyCounter < EnemyList.Num(); EnemyCounter++)
	{
		if (!EnemyList[EnemyCounter] || EnemyList[EnemyCounter]->IsPendingKill())
		{
			EnemyList.RemoveAt(EnemyCounter);
		}
	}
}
