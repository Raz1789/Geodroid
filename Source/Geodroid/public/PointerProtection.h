// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// UNREAL HEADER FILES
#include "CoreMinimal.h"

// MANDATE FILES
#include "PointerProtection.generated.h"

/*****************************************************************************************************************
* CLASS NAME:	UPOINTERPROTECTION
* DESCRIPTION:	This class checks for null pointer and displays a 
*				debug message on the Ouput Log
*****************************************************************************************************************/
UCLASS()
class GEODROID_API UPointerProtection : public UObject
{
	GENERATED_BODY()
public:

	///***********************************************************************************************************///
	///                                       PUBLIC MEMBER FUNCTIONS
	///***********************************************************************************************************///

	///-------------------------------------- CLASS FUNCTIONS ----------------------------------------------------///
	//Check and Log the Pointer uninitialization errors
	static bool CheckAndLog(void* Pointer, FString PointerName);
};
