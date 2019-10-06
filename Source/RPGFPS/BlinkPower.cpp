// Fill out your copyright notice in the Description page of Project Settings.

#include "BlinkPower.h"
#include "RPGFPSCharacter.h"
#include "Camera/CameraComponent.h"
#include "BlinkIndicator.h"
#include "Kismet/GameplayStatics.h"
#include "Runtime/Engine/Classes/Kismet/KismetSystemLibrary.h"
#include "ConstructorHelpers.h"
//#include "DrawDebugHelpers.h"

// Sets default values for this component's properties
UBlinkPower::UBlinkPower()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// Not blinking by default
	bIsBlinking = false;
	bIsTargettingActivated = false;

	// Default values
	DefaultFOV = 90.0f;
	BlinkRange = 1000.0f;
	TargettingRadius = 32.0f;
	CharacterHalfHeight = 100.0f;
	BlinkDuration = 0.2f;
	ClimbDuration = 0.1f;
	InsideWallMargin = -1.25f;
	FOVChangeInterval = 0.016667f;
	bIsWallTopAccessible = false;
	ClimbingHeightLimitModifier = 1.0f;
	IndicatorHeightModifier = 0.0f;
	// Grabs the references of the BP and curve, here so that we counter the infamous UE4 bug where the references are lost upon reopening
	static ConstructorHelpers::FObjectFinder<UClass> BlinkIndicatorClassFinder(TEXT("Class'/Game/Blink/Blueprints/BP_BlinkIndicator.BP_BlinkIndicator_C'"));
	if (BlinkIndicatorClassFinder.Object)
	{
		BlinkIndicatorBP = BlinkIndicatorClassFinder.Object;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("BlinkPower: Blueprint Blink Indicator not found!"));
	}
	static ConstructorHelpers::FObjectFinder<UCurveFloat> CurveFinder(TEXT("/Game/Blink/Blueprints/Curve_BlinkFOVMultiplier"));
	if (CurveFinder.Object)
	{
		FOVMultiplierCurve = CurveFinder.Object;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("BlinkPower: Curve not found!"));
	}
}


// Called when the game starts
void UBlinkPower::BeginPlay()
{
	Super::BeginPlay();

	// TODO : Check the interface implementation

	// Spawn the blink indicator
	Character = Cast<ARPGFPSCharacter>(GetOwner());
	FVector SpawnLocation;
	FRotator SpawnRotation;
	if (Character)
	{
		SpawnLocation = Character->GetActorLocation();
		SpawnRotation = Character->GetActorRotation();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("BlinkPower: Character not found!"));
	}

	if (BlinkIndicatorBP)
	{
		UWorld* const World = GetWorld();
		if (World)
		{
			BlinkIndicator = World->SpawnActor<ABlinkIndicator>(BlinkIndicatorBP, SpawnLocation, SpawnRotation);
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("BlinkPower: BlinkIndicatorBP not set!"));
	}

	// Get the camera component and set the default FOV
	Cam = Character->GetFirstPersonCameraComponent();
	Cam->FieldOfView = DefaultFOV;

	// Create the sweep parameters
	Sphere = FCollisionShape::MakeSphere(TargettingRadius);
	SweepParams = FCollisionQueryParams();
	SweepParams.AddIgnoredActor(Character); // Ignore the character in the sweep

	// Subscribe to the correct events
	Character->OnBlinkTargetting().AddUFunction(this, FName("BlinkTargettingActivation"));
	Character->OnBlinkActivated().AddUFunction(this, FName("BlinkActivated"));

	// Initialize the FOV elapsed time
	Elapsed = 0.0f;
}

// Called every frame
void UBlinkPower::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Update the blink logic if activated
	if (bIsTargettingActivated)
	{
		UpdateBlink();
	}
}

void UBlinkPower::UpdateBlink()
{
	// Update the sphere sweep
	UWorld* const World = GetWorld();
	if (!World)
	{
		UE_LOG(LogTemp, Error, TEXT("BlinkPower: World not found!"));
	}
	FHitResult Hit(1.0f);
	USceneComponent* CamManagerRoot = UGameplayStatics::GetPlayerCameraManager(World, 0)->GetRootComponent();
	FVector Start = Character->GetActorLocation() + Character->GetActorForwardVector() * 5.0f; // Start 5 cm forward
	FVector End = CamManagerRoot->GetComponentLocation() + BlinkRange * CamManagerRoot->GetForwardVector(); // End at the blink range in the camera facing direction			
	World->SweepSingleByChannel(Hit, Start, End, FQuat::Identity, ECollisionChannel::ECC_Visibility, Sphere, SweepParams);
	//DrawDebugSphere(World, Hit.Location, TargettingRadius, 32, FColor::White);

	// Check if we target a wall and if we will blink on top of it
	bIsWallTopAccessible = false;
	FHitResult HitWall(1.0f);
	if (Hit.ImpactNormal.Z <= 0.1f) // We define wall as a plane ~ perpendicular to XY
	{
		// Get more information about the wall's top surface
		FVector EndWall = Hit.ImpactPoint + Hit.ImpactNormal.GetSafeNormal() * InsideWallMargin; // Inside the wall with a margin
		FVector StartWall = EndWall + FVector(0.0f, 0.0f, CharacterHalfHeight * 2.0f); // Above the top of the wall
		World->LineTraceSingleByChannel(HitWall, StartWall, EndWall, ECollisionChannel::ECC_Visibility, SweepParams);
		//DrawDebugLine(World, StartWall, HitWall.Location, FColor::Blue);

		// Treat the hit information
		if (HitWall.bBlockingHit) // The wall top is wide enough
		{
			if (HitWall.ImpactPoint.Z - HitWall.TraceEnd.Z <= CharacterHalfHeight * ClimbingHeightLimitModifier) // The top of the wall is reachable
			{
				// Determine whether or not there is enough space to stand on the wall
				FHitResult HitSpace(1.0f);
				FVector StartSpace = HitWall.ImpactPoint + FVector(0.0f, 0.0f, TargettingRadius + 1.0f); // Add 1.0f to prevent collision with the surface
				FVector EndSpace = HitWall.ImpactPoint + FVector(0.0f, 0.0f, (CharacterHalfHeight - TargettingRadius) * 2.0f); // Gives with the default values ~200 in height, the size of the default player capsule
				World->SweepSingleByChannel(HitSpace, StartSpace, EndSpace, FQuat::Identity, ECollisionChannel::ECC_Visibility, Sphere, SweepParams);
				//DrawDebugSphere(World, HitSpace.Location, TargettingRadius, 32, FColor::Green);

				if (!HitSpace.bBlockingHit) // We have enough space
				{
					bIsWallTopAccessible = true;
				}
			}
		}
	}

	// Determine the position of the blink indicator (below the actual targeting point)
	FHitResult HitIndicator(1.0f);
	FVector StartIndicator = Hit.bBlockingHit ? Hit.Location : Hit.TraceEnd; // Start at the impact or the end of the first trace
	FVector EndIndicator = StartIndicator - FVector(0.0f, 0.0f, CharacterHalfHeight * IndicatorHeightModifier); // End at the half distance below
	World->LineTraceSingleByChannel(HitIndicator, StartIndicator, EndIndicator, ECollisionChannel::ECC_Visibility, SweepParams);
	FVector IndicatorLocation = HitIndicator.bBlockingHit ? HitIndicator.Location : HitIndicator.TraceEnd;
	//DrawDebugLine(World, StartIndicator, HitIndicator.Location, FColor::Red);

	// Move and update the blink indicator, including arrows if the wall top is accessible
	BlinkIndicator->SetActorLocation(IndicatorLocation, false, (FHitResult*)nullptr, ETeleportType::TeleportPhysics);
	if (bIsWallTopAccessible)
	{
		BlinkIndicator->DisableArrows(false);
		BlinkIndicator->AlignArrows(Hit.ImpactNormal);
	}
	else
	{
		BlinkIndicator->DisableArrows(true);
	}

	// Update the destination point: add the half height to take into account the character collider
	DestinationPoint = IndicatorLocation + FVector(0.0f, 0.0f, CharacterHalfHeight);
	if (bIsWallTopAccessible)
	{
		WallTopDestinationPoint = HitWall.ImpactPoint + FVector(0.0f, 0.0f, CharacterHalfHeight);
	}
}

// Blink targetting behaviour
void UBlinkPower::BlinkTargettingActivation()
{
	// Activate the target only if we are not blinking and if it's not already activated
	if (!bIsBlinking && !bIsTargettingActivated)
	{
		BlinkIndicator->DisableActor(false);
		bIsTargettingActivated = true;
	}
}

// Deactivates the indicator
void UBlinkPower::BlinkTargettingDeactivation()
{
	BlinkIndicator->DisableActor(true);
	bIsTargettingActivated = false;
}

// Activates the blink power
void UBlinkPower::BlinkActivated()
{
	// Only continue if the targetting was activated
	if (!bIsTargettingActivated) return;

	// And now, blink!
	if (!bIsBlinking)
	{
		bIsBlinking = true;

		// Update to get the final destination information
		UpdateBlink();

		// Launch effect with a timer
		GetWorld()->GetTimerManager().SetTimer(BlinkTimerHandle, this, &UBlinkPower::OnBlinkEffects, FOVChangeInterval, true, 0.0f);

		// Deactivate the indicator
		BlinkTargettingDeactivation();

		// Move the character
		FLatentActionInfo LatentInfo;
		LatentInfo.CallbackTarget = this;
		if (bIsWallTopAccessible)
		{
			DestinationPoint = WallTopDestinationPoint;
			LatentInfo.ExecutionFunction = FName("ResetIsBlinking");

			//LatentInfo.ExecutionFunction = FName("ClimbWall");
		}
		else
		{
			LatentInfo.ExecutionFunction = FName("ResetIsBlinking");
		}

		LatentInfo.Linkage = 1; // Should not be 0 in order to have the callback
		UKismetSystemLibrary::MoveComponentTo(Character->GetRootComponent(), DestinationPoint, Character->GetRootComponent()->RelativeRotation, false, false, BlinkDuration, false, EMoveComponentAction::Type::Move, LatentInfo);
	}
}

void UBlinkPower::ClimbWall()
{
	// Move the character (IsBlinking is reset at the end)
	FLatentActionInfo LatentInfo;
	LatentInfo.CallbackTarget = this;
	LatentInfo.ExecutionFunction = FName("ResetIsBlinking");
	LatentInfo.Linkage = 1; // Should not be 0 in order to have the callback
	UKismetSystemLibrary::MoveComponentTo(Character->GetRootComponent(), WallTopDestinationPoint, Character->GetRootComponent()->RelativeRotation, false, false, ClimbDuration, false, EMoveComponentAction::Type::Move, LatentInfo);
}

void UBlinkPower::ResetIsBlinking()
{
	bIsBlinking = false;
}

void UBlinkPower::OnBlinkEffects()
{
	Elapsed += FOVChangeInterval;
	if (Elapsed < BlinkDuration)
	{
		Cam->SetFieldOfView(FOVMultiplierCurve->GetFloatValue(Elapsed) * DefaultFOV);
	}
	else
	{
		Elapsed = 0.0f;
		GetWorld()->GetTimerManager().ClearTimer(BlinkTimerHandle);
	}
}
