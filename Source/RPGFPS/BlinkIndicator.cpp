// Fill out your copyright notice in the Description page of Project Settings.

#include "BlinkIndicator.h"
#include "Particles/ParticleSystemComponent.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
ABlinkIndicator::ABlinkIndicator()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ABlinkIndicator::BeginPlay()
{
	Super::BeginPlay();

	// Disable everything at the start
	SetActorActive(false);
}

// Called every frame
void ABlinkIndicator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Function made to enable/disable the actor
void ABlinkIndicator::SetActorActive(bool State)
{
	if (!State)
	{
		for (int i = 0; i < BlinkParticles.Num(); i++)
		{
			BlinkParticles[i]->Deactivate();
		}
	}
	else
	{
		for (int i = 0; i < BlinkParticles.Num(); i++)
		{
			BlinkParticles[i]->Activate(true);
		}
	}

	SetArrowsActive(State);
}

// Function made to enable/disable the direction arrows indicating that we will blink on top of a wall
void ABlinkIndicator::SetArrowsActive(bool State)
{
	for (int i = 0; i < Arrows.Num(); i++)
	{
		Arrows[i]->SetHiddenInGame(!State);
	}
}

