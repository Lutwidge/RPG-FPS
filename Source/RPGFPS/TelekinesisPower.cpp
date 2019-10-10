// Fill out your copyright notice in the Description page of Project Settings.

#include "RPGFPSCharacter.h"
#include "TelekinesisPower.h"

// Sets default values for this component's properties
UTelekinesisPower::UTelekinesisPower()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// Defaults values
	bHasAttractedObject = false;
}


// Called when the game starts
void UTelekinesisPower::BeginPlay()
{
	Super::BeginPlay();

	Character = Cast<ARPGFPSCharacter>(GetOwner());
	if (!Character)
	{
		UE_LOG(LogTemp, Error, TEXT("TelekinesisPower: Character not found!"));
	}
}


// Called every frame
void UTelekinesisPower::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

// Function to implement from the interface
void UTelekinesisPower::SetPowerActive_Implementation(bool State)
{
	if (State)
	{
		// Subscribe to the correct events
		Character->OnPowerPressed().AddUFunction(this, FName("AttractObject"));
		Character->OnPowerReleased().AddUFunction(this, FName("ProjectObject"));
	}
	else
	{
		// Unsubscribe and reset the targetting
		Character->OnPowerPressed().RemoveAll(this);
		Character->OnPowerReleased().RemoveAll(this);
	}
}

// Called when the player presses the power button, tries to attract an object
void UTelekinesisPower::AttractObject()
{

}

// Called when the player releases the power button, project the attracted object if there is one
void UTelekinesisPower::ProjectObject()
{

}

