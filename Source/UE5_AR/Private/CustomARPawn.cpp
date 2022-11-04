// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomARPawn.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "CustomActor.h"
#include "Camera/CameraComponent.h"
#include "CustomGameMode.h"
#include "goghCube.h"
#include "WorldSphere.h"
#include "ARPlaneActor.h"


// Sets default values
ACustomARPawn::ACustomARPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	SetRootComponent(SceneComponent);


	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	
	CameraComponent->SetupAttachment(SceneComponent);


	static ConstructorHelpers::FObjectFinder<UARSessionConfig> ConfigAsset(TEXT("ARSessionConfig'/Game/Blueprints/HelloARSessionConfig.HelloARSessionConfig'"));
	Config = ConfigAsset.Object;

}

// Called when the game starts or when spawned
void ACustomARPawn::BeginPlay()
{
	Super::BeginPlay();

	UARBlueprintLibrary::StartARSession(Config);

}


// Called every frame
void ACustomARPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	handleImageRecognition();

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
	//PlayerInputComponent->BindAxis(APawn::GetMovementComponent);
	//this->GetMovementComponent()
	PlayerInputComponent->BindTouch(IE_Repeat, this, &ACustomARPawn::OnScreenHeld);
	PlayerInputComponent->BindAxis("MoveRight", this, &ACustomARPawn::moveRight);
	PlayerInputComponent->BindAxis("MoveForward", this, &ACustomARPawn::moveForward);

}

void ACustomARPawn::OnScreenHeld(const ETouchIndex::Type FingerIndex, const FVector ScreenPos) {
	if (selectedActor_) {
		auto TraceResult = UARBlueprintLibrary::LineTraceTrackedObjects(FVector2D(ScreenPos), false, false, false, true);
		if (TraceResult.Num() > 0) {
			auto TrackedTF = TraceResult[0].GetLocalToWorldTransform();

			selectedActor_->myPos = TrackedTF.GetLocation();
			//GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Orange, TrackedTF.GetLocation().ToString());
		}
		else {
			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, "Drag function is linetracing outside the plane");
		}
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

void ACustomARPawn::handleImageRecognition() {
	auto storage = UARBlueprintLibrary::GetAllGeometriesByClass<UARTrackedImage>();

	for (int i = 0; i < storage.Num(); i++) {
		if (storage[i]->GetDetectedImage()) {
			if (storage[i]->GetDetectedImage()->GetFriendlyName().Equals("vgoghDA")) {
				auto Tf = storage[i]->GetLocalToTrackingTransform();
				FVector MyLoc(0, 0, 0);
				MyLoc = Tf.GetLocation();

				if (!bGoghFound) {
					const FActorSpawnParameters SpawnInfo;
					const FRotator MyRot(0, 0, 0);
					GoghCube = GetWorld()->SpawnActor<AgoghCube>(MyLoc, MyRot, SpawnInfo);
					GoghCube->SetActorScale3D(FVector(0.1f, 0.1f, 0.1f));
					bGoghFound = true;
					GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, TEXT("Goggy found and spawned cube!"));

				}

				GoghCube->SetActorLocation(MyLoc);

			}
			else if(storage[i]->GetDetectedImage()->GetFriendlyName().Equals("earthDA")) {
				auto Tf = storage[i]->GetLocalToTrackingTransform();
				FVector MyLoc(0, 0, 0);
				MyLoc = Tf.GetLocation();

				if (!bWorldFound) {
					const FActorSpawnParameters SpawnInfo;
					const FRotator MyRot(0, 0, 0);
					WorldCube = GetWorld()->SpawnActor<AWorldSphere>(MyLoc, MyRot, SpawnInfo);
					WorldCube->SetActorScale3D(FVector(0.1f, 0.1f, 0.1f));
					bWorldFound = true;
					GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, TEXT("Goggy found and spawned cube!"));

				}

				WorldCube->SetActorLocation(MyLoc);
			}
		}
	}
}

void ACustomARPawn::moveRight(float amount) {
	if (amount) {
		FVector fwd = GetActorRightVector();
		this->AddActorWorldOffset(fwd * desktopMSspeed);
	}
}
void ACustomARPawn::moveForward(float amount) {
	if (amount){
		FVector fwd = GetActorForwardVector();
		this->AddActorWorldOffset(fwd * desktopMSspeed);

	}
}