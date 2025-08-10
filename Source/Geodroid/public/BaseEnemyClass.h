// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "MapNode.h"
#include "MapClass.h"
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
	class AGeodroidCharacter* Player;

	//Collision capsule
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Game Design")
	class USphereComponent* CollisionComponent = nullptr;

	///-------------------------------------- ENEMY STAT VARIABLES ---------------------------------------------------///
	//Current Health of the Enemy
	float EnemyHealth;

	//Max Health of the Enemy
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Design")
	float MaxEnemyHealth;

	//The amount of gold this enemy gives
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Design")
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

	//Projectile class
	UPROPERTY(Editanywhere, BlueprintReadWrite, Category = "Game Design")
	TSubclassOf<class AGeodroidProjectile> ProjectileClass;

	//Fire Rate of Enemy
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Design")
	float EnemyFireRate;

	//Attack Damage of the Enemy
	UPROPERTY(Editanywhere, BlueprintReadWrite, Category = "Game Design")
	float AttackDamage;

	/** Sound to play each time we fire **/
	UPROPERTY(Editanywhere, BlueprintReadWrite, Category = "Game Design")
	class USoundBase* FireSound;

	//Time Lapsed after last fire
	float TimeFromLastFire;

	///-------------------------------------- VELOCITY VARIABLES -------------------------------------------------///
	
	//Speed of the Enemy
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Design")
	float EnemyVelocity;

	//Influence the speed of Enemy
	float SpeedImpact;

	///-------------------------------- PATHFINDING VARIABLES ---------------------------------------------------///
	//Get a Pathfinding Object for this Enemy
	class UA_Pathfinding* Pathfinder;

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
