// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MapNode.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FMapNode
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Struct Data")
	FVector Position;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Struct Data")
	bool bWalkable;

	void StructInit(FVector _Position, bool _bWalkable);
	
};
