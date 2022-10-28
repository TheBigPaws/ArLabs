// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomARPawn.h"
#include "Runtime/Engine/Classes/Kismet/KismetSystemLibrary.h"
#include "ARBlueprintLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "CustomActor.h"
#include "Camera/CameraComponent.h"
#include "CustomGameMode.h"
#include "ARPlaneActor.h"

#

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
	//UARSessionConfig* Config = NewObject<UARSessionConfig>();
	//UARBlueprintLibrary::StartARSession(Config);
}


// Called every frame
void ACustomARPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//SHOW CAM POS
	//FString CameraPos = "Camera POS: ";
	//APlayerCameraManager* manRef = GetWorld()->GetFirstPlayerController()->PlayerCameraManager;
	//CameraPos += manRef->GetActorTransform().GetLocation().ToString();
	//GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Green, CameraPos);

}

// Called to bind functionality to input
void ACustomARPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);


	//Bind various player inputs to functions
	// There are a few types - BindTouch, BindAxis, and BindEvent.  
	PlayerInputComponent->BindTouch(IE_Pressed, this, &ACustomARPawn::OnScreenTouch);
	PlayerInputComponent->BindTouch(IE_Repeat, this, &ACustomARPawn::OnScreenHeld);

}

void ACustomARPawn::OnScreenHeld(const ETouchIndex::Type FingerIndex, const FVector ScreenPos) {
	if (selectedActor_) {
		auto TraceResult = UARBlueprintLibrary::LineTraceTrackedObjects(FVector2D(ScreenPos), false, false, false, true);
		auto TrackedTF = TraceResult[0].GetLocalToWorldTransform();

		selectedActor_->myPos = TrackedTF.GetLocation();
		//GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Orange, TrackedTF.GetLocation().ToString());
	}
}

void ACustomARPawn::OnScreenTouch(const ETouchIndex::Type FingerIndex, const FVector ScreenPos)
{
	auto Temp = GetWorld()->GetAuthGameMode();
	auto GM = Cast<ACustomGameMode>(Temp);

	//deselect all on new touch
	for (TObjectIterator<APlaceableActor> It; It; ++It)
	{
		APlaceableActor* CurrentObject = *It;
		CurrentObject->selected = false;
		CurrentObject->nearby = true;
		CurrentObject->StaticMeshComponent->SetMaterial(0, CurrentObject->onMat);
		CurrentObject->SetActorScale3D(FVector(1.0f, 1.0f, 1.0f));
		selectedActor_ = NULL;
	}

	
	// Perform a hitTest, get the return values as hitTesTResult
	FHitResult HRRef;
	if (!WorldHitTest(FVector2D(ScreenPos), HRRef))
	{
		// HitTest returned false, get out.
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Orange, TEXT("Nothing pressed"));
		//return;

		for (TObjectIterator<AARPlaneActor> It; It; ++It)
		{
			AARPlaneActor* CurrentObject = *It;
			CurrentObject->shouldBeVisible = true;
		}
	}
	else {
		//UClass* hitActorClass = UGameplayStatics::GetObjectClass(HRRef.GetActor());
		//GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, hitActorClass->GetFName().ToString());
		//GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, HRRef.GetActor()->GetClass()->GetFName().ToString());
		//return;
			APlaceableActor* placActorRef = Cast<APlaceableActor>(HRRef.GetActor());
			if (placActorRef) {
				GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, TEXT("Android clicked"));
				placActorRef->selected = true;
				placActorRef->StaticMeshComponent->SetMaterial(0, placActorRef->selectedMat);
				selectedActor_ = placActorRef;

				for (TObjectIterator<AARPlaneActor> It; It; ++It)
				{
					AARPlaneActor* CurrentObject = *It;
					CurrentObject->shouldBeVisible = false;
				}
				return;
			}
			else {
				GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, TEXT("CAST TO PlaceableActor FAILED"));
				GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, HRRef.GetActor()->GetFName().ToString());
				for (TObjectIterator<AARPlaneActor> It; It; ++It)
				{
					AARPlaneActor* CurrentObject = *It;
					CurrentObject->shouldBeVisible = true;
				}
				return;
			}
		
	}

	// Get object of actor hit.
	//UClass* hitActorClass = UGameplayStatics::GetObjectClass(HRRef.GetActor());
	// if we've hit a target.

	//GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, FString(HRRef.GetActor()->GetClass()));
	//UKismetSystemLibrary::PrintString(this, HRRef.GetActor()->GetClass().tostr, true, true, FLinearColor(0, 0.66, 1, 1), 2);


	//APlaceableActor* placActorRef = Cast<APlaceableActor>(HRRef.GetActor());

	
	if (GM)
	{
		GM->LineTraceSpawnActor(ScreenPos);
	}

	
}

bool ACustomARPawn::WorldHitTest(FVector2D screenTouchPos, FHitResult & hitResult) {
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
		//GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, TEXT("HIT SUCCESS"));
	}else{
		//GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, TEXT("NO HIT"));
	}
	// return if the operation was successful
	return traceSuccess;
}