// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MapNode.generated.h"

USTRUCT(BlueprintType)
//Struct containing the Map Nodes
struct FMapNode
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Struct Data")
		FVector2D NodeIndex;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Struct Data")
		FVector Position;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Struct Data")
		bool bWalkable;

	void StructInit(FVector2D _NodeIndex, FVector _Position, bool _bWalkable);

	//Operator "==" for comparing
	bool operator== (const FMapNode& other) const;
	
};
