// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomARPawn.h"
#include "Runtime/Engine/Classes/Kismet/KismetSystemLibrary.h"
#include "ARBlueprintLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "CustomActor.h"
#include "Camera/CameraComponent.h"
#include "CustomGameMode.h"

// Sets default values
ACustomARPawn::ACustomARPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	SetRootComponent(SceneComponent);


	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	
	CameraComponent->SetupAttachment(SceneComponent);
}

// Called when the game starts or when spawned
void ACustomARPawn::BeginPlay()
{
	Super::BeginPlay();
	UARSessionConfig* Config = NewObject<UARSessionConfig>();
	UARBlueprintLibrary::StartARSession(Config);
}


// Called every frame
void ACustomARPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void ACustomARPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);


	//Bind various player inputs to functions
	// There are a few types - BindTouch, BindAxis, and BindEvent.  
	PlayerInputComponent->BindTouch(IE_Pressed, this, &ACustomARPawn::OnScreenTouch);

	//layerInputComponent->BindTouch(IE_Pressed, this, &ACustomARPawn::OnScreenTouch);
}

void ACustomARPawn::OnScreenTouch(const ETouchIndex::Type FingerIndex, const FVector ScreenPos)
{
	auto Temp = GetWorld()->GetAuthGameMode();
	auto GM = Cast<ACustomGameMode>(Temp);

	//GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, TEXT("ScreenTouch Reached"));
	if(GM)
	{
		// Add call to the line-trace here from the Custom Game Mode
	}
	// Perform a hitTest, get the return values as hitTesTResult
	FHitResult HRRef;
	if (!WorldHitTest(FVector2D(ScreenPos), HRRef))
	{
		// HitTest returned false, get out.
		UKismetSystemLibrary::PrintString(this, "Nothing pressed", true, true, FLinearColor(0, 0.66, 1, 1), 2);
		return;
	}
	// Get object of actor hit.
	UClass* hitActorClass = UGameplayStatics::GetObjectClass(HRRef.GetActor());
	// if we've hit a target.
	if (UKismetMathLibrary::ClassIsChildOf(hitActorClass, ACustomActor::StaticClass()))
	{
		UKismetSystemLibrary::PrintString(this, "Cube clicked!", true, true, FLinearColor(0, 0.66, 1, 1), 2);
	}
}

bool ACustomARPawn::WorldHitTest(FVector2D screenTouchPos, FHitResult hitResult) {
	// Get player controller
	APlayerController* playerController = UGameplayStatics::GetPlayerController(this, 0);
	// Perform deprojection taking 2d clicked area and generating reference in 3d world-space.
	FVector worldPosition;
	FVector worldDirection; // Unit Vector
	
	bool deprojectionSuccess = UGameplayStatics::DeprojectScreenToWorld(playerController, screenTouchPos, /*out*/
		worldPosition, /*out*/ worldDirection);
	
	// construct trace vector (from point clicked to 1000.0 units beyond in same direction)
	FVector traceEndVector = worldDirection * 1000.0;
	traceEndVector = worldPosition + traceEndVector;
	// perform line trace (Raycast)
	bool traceSuccess = GetWorld()->LineTraceSingleByChannel(hitResult, worldPosition, traceEndVector, ECollisionChannel::ECC_WorldDynamic);

	if (traceSuccess) {
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, TEXT("HIT SUCCESS"));
	}else{
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, TEXT("NO HIT"));
	}
	// return if the operation was successful
	return traceSuccess;
}