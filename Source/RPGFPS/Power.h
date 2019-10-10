// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Power.generated.h"

// BlueprintType so that the interface can be exposed to blueprints
UINTERFACE(BlueprintType)
class UPower : public UInterface
{
	GENERATED_BODY()
};

/**
 * Power interface
 */
class RPGFPS_API IPower
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	// Classes using this interface MUST implement this function
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Power")
	void SetPowerActive(bool State);
};
