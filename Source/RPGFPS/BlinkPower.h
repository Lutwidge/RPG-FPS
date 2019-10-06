// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

class ABlinkIndicator;
class UCameraComponent;
class ARPGFPSCharacter;
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BlinkPower.generated.h"


UCLASS( ClassGroup=(Blink), meta=(BlueprintSpawnableComponent) )
class RPGFPS_API UBlinkPower : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UBlinkPower();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	// Blink targetting behaviour
	UFUNCTION()
	void BlinkTargettingActivation();

	// Deactivates the indicator
	void BlinkTargettingDeactivation();

	// Activates the blink power
	UFUNCTION()
	void BlinkActivated();

	// Update all the blink data
	void UpdateBlink();

	// Go on top of the wall after blinking straight
	UFUNCTION(BlueprintCallable)
	void ClimbWall();

	// Reset blink status (has to be blueprint callable for the callback to work)
	UFUNCTION(BlueprintCallable)
	void ResetIsBlinking();

	// Change FOV during the blink
	UFUNCTION()
	void OnBlinkEffects();

protected:
	// Blueprint class of the blink indicator
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Blink)
	TSubclassOf<ABlinkIndicator> BlinkIndicatorBP;

	// Default FOV during the dash
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Blink)
	float DefaultFOV;

	// Range of the blink power
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Blink)
	float BlinkRange;

	// Radius of the sphere sweep for targetting
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Blink)
	float TargettingRadius;

	// Half-height of the character for the top of the walls collision check
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Blink)
	float CharacterHalfHeight;

	// Speed of the blink
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Blink)
	float BlinkDuration;

	// Speed of the climbing of the walls
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Blink)
	float ClimbDuration;

	// Margin when sweeping inside the wall to check if we can go on top of it
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Blink)
	float InsideWallMargin;

	// Curve of the FOV change
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Blink)
	UCurveFloat* FOVMultiplierCurve;

	// Update speed of the FOV change
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Blink)
	float FOVChangeInterval;

	// Changes the height difference needed to climb a wall with the blink (1.0f => CharacterHalfHeight)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Blink)
	float ClimbingHeightLimitModifier;

	// Changes the height difference between the targetting and the indicator (1.0f => CharacterHalfHeight)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Blink)
	float IndicatorHeightModifier;

private:
	// Reference to the blink indicator that will be spawned
	ABlinkIndicator* BlinkIndicator;

	// Reference to the camera component of the character
	UCameraComponent* Cam;

	// Reference to the character the power is linked to
	ARPGFPSCharacter* Character;

	// Is the character in the middle of the blink process
	bool bIsBlinking;

	// Is the blink targetting activated
	bool bIsTargettingActivated;

	// Sweep parameters
	FCollisionShape Sphere;
	FCollisionQueryParams SweepParams;

	// Blink destinations
	FVector DestinationPoint;
	FVector WallTopDestinationPoint;

	// Can the player go on top of the wall
	bool bIsWallTopAccessible;

	// Handle to manage the FOV change
	FTimerHandle BlinkTimerHandle;

	// Timer in the FOV change
	float Elapsed;
};
