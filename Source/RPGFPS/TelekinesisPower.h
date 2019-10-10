// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

class UCameraComponent;
class ARPGFPSCharacter;
#include "Power.h"
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TelekinesisPower.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RPGFPS_API UTelekinesisPower : public UActorComponent, public IPower
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UTelekinesisPower();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:
	// Called when the player presses the power button, tries to attract an object
	void AttractObject();

	// Called when the player releases the power button, project the attracted object if there is one
	void ProjectObject();

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// Function to implement from the interface
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Power")
	void SetPowerActive(bool State); // This tells the engine that we can call and override this function in blueprints
	virtual void SetPowerActive_Implementation(bool State) override; // Actual implementation

private:
	// Reference to the character the power is linked to
	ARPGFPSCharacter* Character;

	// Is an object attracted ?
	bool bHasAttractedObject;
};
