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
	UPROPERTY(EditDefaultsOnly, Category = "Turret Design")
	//Influence Circle radius value for Game Design Purposes
	float InfluenceCircleRadius;

	UPROPERTY(EditDefaultsOnly, Category = "Turret Design")
	//Inner Collision Box representing the Turret Area Restriction
	UBoxComponent* RestrictionArea;

	//CollisionShape for Sweeping for enemies
	FCollisionShape InfluenceSphere;

	//Array of Enemies inside the InfluenceBox
	TArray<FHitResult> OutHits;

	//Has the sweep Hit any objects
	bool bHasSweepHitAnyObject;

	///-------------------------------------- FIRING VARIABLE ----------------------------------------------------///
	//Stores the Target Actor
	AActor* TargetActor = nullptr;

	//Time from last shot was fired
	float TimeFromLastFire; 
	
	UPROPERTY(EditDefaultsOnly, Category = "Game Design")
	//Projectile class
	TSubclassOf<class AGeodroidProjectile> ProjectileClass;

	UPROPERTY(EditDefaultsOnly, Category = "Game Design")
	/** Sound to play each time we fire */
	class USoundBase* FireSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Game Design")
	//Turret Rotator
	UStaticMeshComponent* BP_Turret;

	///***********************************************************************************************************///
	///                                       PROTECTED MEMBER FUNCTIONS
	///***********************************************************************************************************///
	
	///-------------------------------------- OVERRIDE FUNCTIONS -------------------------------------------------///
	virtual void BeginPlay() override;

	///-------------------------------------- CLASS FUNCTIONS ----------------------------------------------------///
	UFUNCTION(BlueprintImplementableEvent, Category = "CPP Function", meta =  (DisplayName = "LookAtTargetEnemy"))
		//Rotate Turret to Look at Target Enemy
		void ReceiveLookAtTargetEnemy(const AActor* TargetEnemy);

	UFUNCTION()
	//Check if the Enemy inside the InfluenceBox is visible and take necessary action
	void SearchForEnemy();

	UFUNCTION()
		//Shoot at Enemy
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
