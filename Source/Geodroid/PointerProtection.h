// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PointerProtection.generated.h"

UCLASS()
class GEODROID_API UPointerProtection : public UObject
{
	GENERATED_BODY()
public:

	//Check and Log the Pointer uninitialization errors
	static bool CheckAndLog(void* Pointer, FString PointerName);
};
