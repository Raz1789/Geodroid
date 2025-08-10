// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// UNREAL HEADER FILES
#include "CoreMinimal.h"
#include "WorldCollision.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"

// PROJECT HEADER FILES
#include "DefenseStructures.h"
#include "GeodroidProjectile.h"
#include "PointerProtection.h"
#include "MapClass.h"

// MANDATE FILES
#include "Turret.generated.h"

/*****************************************************************************************************************
* CLASS NAME:	ATURRET
* DESCRIPTION:	This class is a derived class from the ADefenseStructure class.
*				This class specifies the behaviour of the Turret in the game
*               The Turret tracks the enemy that is in the Influence Circle and shoots
*				at the enemy only if the enemy is visible
*****************************************************************************************************************/

UCLASS()
class GEODROID_API ATurret : public ADefenseStructures
{
	GENERATED_BODY()

protected:
	///***********************************************************************************************************///
	///                                       PROTECTED MEMBER VARIABLE
	///***********************************************************************************************************///

	///-------------------------------------- COMMON VARIABLES ---------------------------------------------------///
	//The World Component
	UWorld* World;

	//The Actor Location
	FVector ActorLocation;

	//Actor Node in Map
	FVector2D CurrentMapNode;

	///-------------------------------------- SWEEP VARIABLES ----------------------------------------------------///
	//Influence Circle radius value for Game Design Purposes
	UPROPERTY(EditDefaultsOnly, Category = "Turret Design")
	float InfluenceCircleRadius;

	//Inner Collision Box representing the Turret Area Restriction
	UPROPERTY(EditDefaultsOnly, Category = "Turret Design")
	UBoxComponent* RestrictionArea;

	//CollisionShape for Sweeping for enemies
	FCollisionShape InfluenceSphere;

	//Array of Enemies inside the InfluenceBox
	TArray<FHitResult> OutHits;

	//Has the sweep Hit any objects
	bool bHasSweepHitAnyObject;

	///-------------------------------------- FIRING VARIABLE ----------------------------------------------------///
	//Stores the Target Actor
	AActor* Target = nullptr;

	//Time from last shot was fired
	float TimeFromLastFire;

	//Projectile class
	UPROPERTY(EditDefaultsOnly, Category = "Game Design")
	TSubclassOf<class AGeodroidProjectile> ProjectileClass;

	/** Sound to play each time we fire */
	UPROPERTY(EditDefaultsOnly, Category = "Game Design")
	class USoundBase* FireSound;

	//Turret Rotator
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Game Design")
	UStaticMeshComponent* BP_Turret;

	///***********************************************************************************************************///
	///                                       PROTECTED MEMBER FUNCTIONS
	///***********************************************************************************************************///

	///-------------------------------------- OVERRIDE FUNCTIONS -------------------------------------------------///
	virtual void BeginPlay() override;

	///-------------------------------------- CLASS FUNCTIONS ----------------------------------------------------///
	//Rotate Turret to Look at Target Enemy
	UFUNCTION(BlueprintImplementableEvent, Category = "CPP Function", meta = (DisplayName = "LookAtTargetEnemy"))
	void ReceiveLookAtTargetEnemy(const AActor* TargetEnemy);

	//Check if the Enemy inside the InfluenceBox is visible and take necessary action
	UFUNCTION()
	void SearchForEnemy();

	//Shoot at Enemy
	UFUNCTION()
	void ShootAtEnemy(const  AActor* TargetEnemy);

	UFUNCTION()
	bool IsEnemyInVisibleRange(const  AActor* TargetEnemy);

public:

	///***********************************************************************************************************///
	///                                       PUBLIC MEMBER FUNCTIONS
	///***********************************************************************************************************///

	///-------------------------------------- CONSTRUCTOR --------------------------------------------------------///
	ATurret();

	///-------------------------------------- OVERRIDE FUNCTIONS -----------------------------------------------///
	//Tick function
	virtual void Tick(float DeltaTime) override;
};
