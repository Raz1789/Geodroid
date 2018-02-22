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
	StartNode = FVector2D(0.f, 0.f);
	TargetNode = FVector2D(10.f, 5.f);

	///ENEMY DESIGN VARIABLES
	TimeBetweenSpawn = 1.f;
	TimeFromLastSpawn = 0.f;
	PawnCounter = 0;
	MaxSpawnablePawns = 5;
}

///FPS TEMPLATE CODE MODIFIED FROM HERE ON BELOW

void AGeodroidGameMode::BeginPlay()
{
	Super::BeginPlay();

	

	UMapClass::Init(MapDesignWalkableArray, NodeWorldSize, TargetNode, MapMaxSize);

	if (UPointerProtection::CheckAndLog(GetWorld(), "World"))
	{
		World = GetWorld();
	}
	
}

void AGeodroidGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (PawnCounter < MaxSpawnablePawns)
	{
		TimeFromLastSpawn += DeltaTime;
	}

	//Check if the SpawnDelay is crossed && if PawnCounter is less than the MAX_SPAWNED_PAWN
	if ((TimeFromLastSpawn > TimeBetweenSpawn) && PawnCounter < MaxSpawnablePawns)
	{
		//Reset CurrentTimer
		TimeFromLastSpawn = 0.f;
		//Spawn an Enemy
		SpawnEnemy(0);
		//Increment PawnCounter
		PawnCounter++;
	}
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