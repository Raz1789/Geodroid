// Fill out your copyright notice in the Description page of Project Settings.

#include "PointerProtection.h"

///***********************************************************************************************************///
///             CHECK IF THE POINTER PASSED IS NULL AND PRINT DEBUG MESSAGE IF IT IS NULL
///				RETURNS TRUE IF IT IS NOT NULL
///***********************************************************************************************************///
//CHECK IF THE POINTER PASSED IS NULL AND PRINT DEBUG MESSAGE IF IT IS NULL (returns bool)
bool UPointerProtection::CheckAndLog(void* Pointer, FString PointerName)
{
	if (Pointer)
	{
		return true;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("%s is a null pointer"), *PointerName);
		return false;
	}
}
