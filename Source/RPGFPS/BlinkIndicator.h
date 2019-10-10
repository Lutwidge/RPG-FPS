// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BlinkIndicator.generated.h"

UCLASS()
class RPGFPS_API ABlinkIndicator : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABlinkIndicator();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Function made to enable/disable the actor
	void SetActorActive(bool State);

	// Function made to enable/disable the direction arrows indicating that we will blink on top of a wall
	void SetArrowsActive(bool State);

protected:
	// Arrow pointer meshes
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = BlinkIndicator)
	TArray<UStaticMeshComponent*> Arrows;

	// Arrow pointer meshes
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = BlinkIndicator)
	TArray<UParticleSystemComponent*> BlinkParticles;
};
