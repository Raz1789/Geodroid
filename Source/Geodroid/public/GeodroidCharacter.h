// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once


#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MapNode.h"
#include "NodeViewerActor.h"
#include "GeodroidCharacter.generated.h"

//SELECT THE DEFENSE STRUCTURE FOR SPAWNING
UENUM()
enum class ESelectDefenseStructure : uint8
{
	ESDS_Turret = 0,
	ESDS_Trap = 1
};

class UInputComponent;

/*****************************************************************************************************************
* CLASS NAME:	AGEODROIDCHARACTER
* DESCRIPTION:	Class describing the behaviour of the character
*****************************************************************************************************************/
UCLASS(config=Game)
class AGeodroidCharacter : public ACharacter
{
	GENERATED_BODY()


#pragma region NON MODIFIED VARIABLES AND FUNCTIONS
	/** Pawn mesh: 1st person view (arms; seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category=Mesh)
	class USkeletalMeshComponent* Mesh1P;

	/** Gun mesh: 1st person view (seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	class USkeletalMeshComponent* FP_Gun;

	/** Location on gun mesh where projectiles should spawn. */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	class USceneComponent* FP_MuzzleLocation;

	/** Gun mesh: VR view (attached to the VR controller directly, no arm, just the actual gun) */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	class USkeletalMeshComponent* VR_Gun;

	/** Location on VR gun mesh where projectiles should spawn. */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	class USceneComponent* VR_MuzzleLocation;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FirstPersonCameraComponent;

public:
	//Constructor
	AGeodroidCharacter();

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

	/** Gun muzzle's offset from the characters location */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
	FVector GunOffset;

	/** Projectile class to spawn */
	UPROPERTY(EditDefaultsOnly, Category=Projectile)
	TSubclassOf<class AGeodroidProjectile> ProjectileClass;

	/** Sound to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
	class USoundBase* FireSound;

	/** AnimMontage to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	class UAnimMontage* FireAnimation;

protected:

	virtual void BeginPlay();
	/** Fires a projectile. */
	void OnFire();

	/** Handles moving forward/backward */
	void MoveForward(float Val);

	/** Handles stafing movement, left and right */
	void MoveRight(float Val);

	/**
	 * Called via input to turn at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);
	
protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	// End of APawn interface

#pragma endregion

protected:
	///***********************************************************************************************************///
	///                                       PROTECTED MEMBER VARIABLE
	///***********************************************************************************************************///

	///-------------------------------------- COMMON VARIABLES ---------------------------------------------------///

	//World pointer
	class UWorld* World;

	//The PlayerController
	class APlayerController* PlayerController;

	//The Player HUD
	class AGeodroidHUD* PlayerHUD;
	
	///-------------------------------------- PLAYER STATUS VARIABLES ---------------------------------------------///
	//Health of the player
	float PlayerHealth;
	
	//Is player Dead
	bool bIsPlayerDead;

	//The Node in which the character is standing
	FMapNode CharacterMapNode;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Game Design | Basic")
		//Amount of gold player has.
		int32 PlayerGold;

	UPROPERTY(EditDefaultsOnly, Category = "debug")
		TSubclassOf<ANodeViewerActor> NodeViewerClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Game Design | Basic")
		float PlayerMaxHealth;

	///-------------------------------------- GAME MODE VARIABLES ---------------------------------------------///
	//Start Wave Variable
	bool bShouldWaveStart;

	//If GameMode is ready to StartWave
	bool bIsGameModeReady;


	///-------------------------------------- FIRING VARIABLES ---------------------------------------------///
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Game Design | Basic")
	//Attack Damage per shot from the player
	float AttackDamage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Game Design | StructureConstruction")
		//CrossHair Location in Range 0 - 1 : Default (0.5,0.5)
		FVector2D CrossHairLocation;

	///-------------------------------------- STRUCTURE VARIABLES ---------------------------------------------///
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Game Design | StructureConstruction")
	//Defense structure that is selected
	ESelectDefenseStructure SelectedDefenseStructure;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Game Design | StructureConstruction")
	//Structure Construction range
	float StructureConstructionRange;

	///Variable for construction purposes
	//bool to store if the Construction of Defense Structure feasible
	bool bIsConstructionFeasible;

	UPROPERTY(EditDefaultsOnly, Category = "Game Design | StructureConstruction")
	//TArray containing all the Defense Structures
	TArray<TSubclassOf<class ADefenseStructures>> DefenseStructuresClasses;

	//TArray containing all the Defense Structures
	TArray<ADefenseStructures*> DefenseStructuesSpawnList;

	///***********************************************************************************************************///
	///                                       PROTECTED MEMBER FUNCTION
	///***********************************************************************************************************///

	///-------------------------------------- CLASS FUNCTIONS ---------------------------------------------------///

	//Function to check feasibility of Construction of Defense Structure
	void StructurePlacement();

	//SubFunction of StructurePlacement
	void SpawnStructure(FVector2D &FloorNodeIndex, FVector &FloorPosition);

	//DestroyStructure
	void DestroyStructure(ADefenseStructures* Structure);

	//Select Turret for Construction Action
	void SelectTurretForConstruction();

	//Select Trap for Construction Action
	void SelectTrapForConstruction();

	//Set SelectDefenseStructure variable to StructureConstruction Status
	void DestroyInit();

	//Checks if the Actor provided is visible
	bool VisibilityCheck(const  AActor* Target);

	//Checks if the Actor provided is visible
	AActor* LineTraceForward();

	//Checks if the Actor provided is floor
	AActor* FloorCheck();

	//true when StartWave button is pressed
	void StartWave();

	//Get the Camera Location and LookDirection
	void GetCameraDetails(FVector& OutCameraLocation, FVector& OutCameraLookDirection);

public:
	/** Returns Mesh1P subobject **/
	FORCEINLINE class USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
	/** Returns FirstPersonCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

	///***********************************************************************************************************///
	///                                       PUBLIC MEMBER FUNCTION
	///***********************************************************************************************************///

	///-------------------------------------- CLASS FUNCTIONS ---------------------------------------------------///

	//DEBUG FUNCTION
	void DebugFunction();

	//Function used to add resources from the Enemies
	void AddGold(int32 AmountToBeReceived);

	//Function used to add resources from the Enemies
	bool SubtractGold(int32 AmountToBeDeducted);

	UFUNCTION(BlueprintPure, Category = "CPP Functions")
	//Getter for PlayerGold
	int32 GetPlayerGold() const;

	UFUNCTION(BlueprintPure, Category = "CPP Functions")
	//Returns the PlayerHealth
	float GetPlayerHealth() const;

	//Apply required damage to the Player
	void ApplyDamage(float Damage);

	//Getter for bIsPlayerDead
	bool IsPlayerDead();

	//Get and Reset bShouldWaveStart by GameMode
	bool ShouldWaveStart();
	void ResetStartWave();

	//Set and Reset for bIsGameModeReady for Wave Start
	void SetIsGameModeReady();
	void ResetIsGameModeReady();

	UFUNCTION(BlueprintCallable, Category = "CPP Functions")
		//Function to activate if the Player is hit
		void OnHit(UPrimitiveComponent* HitComp, 
				   AActor* OtherActor, 
				   UPrimitiveComponent* OtherComp, 
				   FVector NormalImpulse, 
				   const FHitResult& Hit);

};

