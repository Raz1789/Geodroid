// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// UNREAL HEADER FILES
#include "CoreMinimal.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Pawn.h"
#include "DrawDebugHelpers.h"
#include "Components/SphereComponent.h"
#include "Engine/World.h"

// PROJECT HEADER FILES
#include "PointerProtection.h"
#include "MapNode.h"
#include "A_Pathfinding.h"
#include "GeodroidProjectile.h"
#include "MapClass.h"
#include "GeodroidCharacter.h"

// MANDATE FILES
#include "BaseEnemyClass.generated.h"


//The Enemy State Enum
UENUM()
enum class EnemyState : uint8
{
	FollowPath,
	FollowPlayer,
	Dead
};

/*****************************************************************************************************************
* CLASS NAME:	ABASEENEMYCLASS
* DESCRIPTION:	This is the base class for all the enemies
				Follows the State Machine style of execution
				Enemy customizable from the Blueprint
*****************************************************************************************************************/

UCLASS()
class GEODROID_API ABaseEnemyClass : public APawn
{
	GENERATED_BODY()

protected:
	///***********************************************************************************************************///
	///                                       PROTECTED MEMBER VARIABLE
	///***********************************************************************************************************///

	///-------------------------------------- COMMON VARIABLES ---------------------------------------------------///
	//Get the World
	UWorld* World;

	//The Player Actor
	AGeodroidCharacter* Player;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Game Design")
	//Collision capsule
	USphereComponent* CollisionComponent = nullptr;

	///-------------------------------------- ENEMY STAT VARIABLES ---------------------------------------------------///
	//Current Health of the Enemy
	float EnemyHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Design")
		//Max Health of the Enemy
		float MaxEnemyHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Design")
		//The amount of gold this enemy gives
		int32 EnemyGold;

	//Variable storing the current Node of the Pawn
	FMapNode CurrentNode;

	//Current Enemy State
	EnemyState CurrentState;

	//Previous Enemy State
	EnemyState PreviousState;

	//True when enemy successfully reaches the Endpoint
	bool bReachedEndPointSuccessfully;

	//True when GameMode retrieved the information on reaching the endpoint
	bool bGameModeRetrievedInformation;

	///-------------------------------------- PROJECTILE VARIABLES --------------------------------------------///

	UPROPERTY(Editanywhere, BlueprintReadWrite, Category = "Game Design")
		//Projectile class
	TSubclassOf<class AGeodroidProjectile> ProjectileClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Design")
		//Fire Rate of Enemy
		float EnemyFireRate;

	UPROPERTY(Editanywhere, BlueprintReadWrite, Category = "Game Design")
		//Attack Damage of the Enemy
		float AttackDamage;

	/** Sound to play each time we fire **/
	UPROPERTY(Editanywhere, BlueprintReadWrite, Category = "Game Design")
		class USoundBase* FireSound;

	//Time Lapsed after last fire
	float TimeFromLastFire;

	///-------------------------------------- VELOCITY VARIABLES -------------------------------------------------///
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Design")
	//Speed of the Enemy
	float EnemyVelocity;

	//Influence the speed of Enemy
	float SpeedImpact;

	///-------------------------------- PATHFINDING VARIABLES ---------------------------------------------------///
	//Get a Pathfinding Object for this Enemy
	UA_Pathfinding* Pathfinder;

	//Contain the path that the pawn should follow
	TArray<FVector2D> PathList;

	//A counter storing the index of the target node from the PathList
	int32 PathCounter;

	///-------------------------------- PATHFINDING VARIABLES ---------------------------------------------------///
	struct TimerContainer
	{
		float MaxTime;
		float CurrentTime;
	};

	//Custom Time Container for setting the DeathtimeCounter
	TimerContainer DeathTimer;

	///***********************************************************************************************************///
	///                                       PROTECTED MEMBER FUNCTION
	///***********************************************************************************************************///

	///-------------------------------------- OVERRIDE FUNCTIONS -------------------------------------------------///
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	///-------------------------------------- CLASS FUNCTIONS ----------------------------------------------------///
	//Move to next Node Function
	void MovePawnAlongPathList(float DeltaTime);

	//Check if End Target node has reac
	void CheckTargetReached(float EndDistance);

	//Move Enemy to the Target Vector provided
	void MoveToTargetVector(const FVector& TargetVector);

	//Follow the player
	void FollowPlayer(float EndDistance, float DeltaTime);

	//Fire at player if distance is less
	void ShootAtPlayer();

	//Check if player visible and in Range
	bool IsPlayerInVisibleRange();

	//Initial the death sequence
	void DeathSequence(float DeltaTime);

	//Function to sent Gold to Player
	void SendResourceToPlayer();

public:
	///***********************************************************************************************************///
	///                                       PUBLIC MEMBER FUNCTION
	///***********************************************************************************************************///

	///-------------------------------------- CONSTRUCTOR --------------------------------------------------------///
	// Sets default values for this pawn's properties
	ABaseEnemyClass();

	///-------------------------------------- OVERRIDE FUNCTIONS -------------------------------------------------///
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	///-------------------------------------- CLASS FUNCTIONS ----------------------------------------------------///
	UFUNCTION(BlueprintCallable)
	//Get Current Health from 0 - 1 float
	float GetHealth();
	
	//Reduce speed by half (Called from Slow Trap)
	void AddSpeedEffect();

	//Return speed back (Called from Slow Trap)
	void ResetSpeedEffect();
	
	//Function to Update the PathList (Called from GameMode)
	void UpdatePathList(FVector2D TargetNodeIndex = UMapClass::GetTargetNode().NodeIndex);

	//Getter for ReachedEndPointSuccessfully (Called from GameMode)
	bool HasEnemyReachedEndpoint();

	//Getter for ReachedEndPointSuccessfully (Called from GameMode)
	void ResetReachedEndPointSuccessfully();

	//Subtracts damage from health
	void ApplyDamage(float Damage);

	UFUNCTION()
	//Function to activate if the Enemy is hit
	void OnHit(UPrimitiveComponent* HitComp,
			   AActor* OtherActor,
			   UPrimitiveComponent* OtherComp,
			   FVector NormalImpulse,
			   const FHitResult& Hit);	
};
