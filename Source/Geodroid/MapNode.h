// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MapNode.generated.h"



/*****************************************************************************************************************
* STRUCT NAME:	FMAPNODE
* DESCRIPTION:	Struct used for keeping all the essential informations
*				about the Map Node i.e. position, NodeIndex, and whether
*				the node is walkable or not
*****************************************************************************************************************/
USTRUCT(BlueprintType)
struct FMapNode
{
	GENERATED_BODY()

	///***********************************************************************************************************///
	///                                       MEMBER VARIABLE
	///***********************************************************************************************************///
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Struct Data")
	FVector2D NodeIndex;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Struct Data")
		FVector Position;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Struct Data")
		bool bWalkable;


	///***********************************************************************************************************///
	///                                       MEMBER FUNCTONS
	///***********************************************************************************************************///
	//Structure initialization function
	void StructInit(FVector2D _NodeIndex, FVector _Position, bool _bWalkable);

	///-------------------------------------- OVERRIDE FUNCTIONS -------------------------------------------------///
	//Operator "==" overloading for custom comparing
	bool operator== (const FMapNode& Other) const;

	//Operator "!=" overloading for custom comparing
	bool operator!= (const FMapNode& Other) const;

};
