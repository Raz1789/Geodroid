// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// UNREAL HEADER FILES
#include "CoreMinimal.h"
#include "Components/BoxComponent.h"

// PROJECT HEADER FILES
#include "DefenseStructures.h"
#include "BaseEnemyClass.h"

// MANDATE FILES
#include "SlowTrap.generated.h"

/*****************************************************************************************************************
* CLASS NAME:	ASLOWTRAP
* DESCRIPTION:	This class is a derived class from the ADefenseStructure class.
*				This class specifies the behaviour of the Slow Trap in the game
*				The Slow Trap slowes all enemies stepping on it
*****************************************************************************************************************/

UCLASS()
class GEODROID_API ASlowTrap : public ADefenseStructures
{
	GENERATED_BODY()

protected:
	///***********************************************************************************************************///
	///                                       PROTECTED MEMBER VARIABLE
	///***********************************************************************************************************///

	///-------------------------------------- COMMON VARIABLES ---------------------------------------------------///
	//The World Component
	UWorld* World;

	///-------------------------------------- DESIGN VARIABLES ---------------------------------------------------///
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Design")
		//Overlap box
		UBoxComponent* InfluenceBox;

	///***********************************************************************************************************///
	///                                       PROTECTED MEMBER FUNCTIONS
	///***********************************************************************************************************///
	///-------------------------------------- CLASS FUNCTIONS ----------------------------------------------------///
	UFUNCTION()
		void OnBeginOverlap(AActor* OverlappedActor, AActor* OtherActor);

	UFUNCTION()
		void OnEndOverlap(AActor* OverlappedActor, AActor* OtherActor);

public:

	///***********************************************************************************************************///
	///                                       PUBLIC MEMBER FUNCTIONS
	///***********************************************************************************************************///

	///-------------------------------------- CONSTRUCTOR --------------------------------------------------------///
	ASlowTrap();

};
