// Fill out your copyright notice in the Description page of Project Settings.


#include "PlaceableActor.h"
#include "ARPin.h"
#include "ARBlueprintLibrary.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
APlaceableActor::APlaceableActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	SetRootComponent(SceneComponent);


	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	StaticMeshComponent->SetupAttachment(SceneComponent);

	auto MeshAsset = ConstructorHelpers::FObjectFinder<UStaticMesh>(TEXT("StaticMesh'/Game/Assets/Meshes/Andy_Mesh.Andy_Mesh'"));
	StaticMeshComponent->SetStaticMesh(MeshAsset.Object);
	
	onMat = CreateDefaultSubobject<UMaterialInstanceDynamic>(TEXT("OnMaterial"));
	offMat = CreateDefaultSubobject<UMaterialInstanceDynamic>(TEXT("OffMaterial"));
	selectedMat = CreateDefaultSubobject<UMaterialInstanceDynamic>(TEXT("SelectedMat"));


	
	static ConstructorHelpers::FObjectFinder<UMaterial> Material_(TEXT("Material'/Game/Assets/Materials/greenMat.greenMat'"));
	UMaterial* settingMaterial_ = (UMaterial*)Material_.Object;
	onMat = UMaterialInstanceDynamic::Create(settingMaterial_, NULL);

	static ConstructorHelpers::FObjectFinder<UMaterial> Material(TEXT("Material'/Game/Assets/Materials/redMat.redMat'"));
	UMaterial* settingMaterial = (UMaterial*)Material.Object;
	offMat = UMaterialInstanceDynamic::Create(settingMaterial, NULL);

	static ConstructorHelpers::FObjectFinder<UMaterial> Material_s(TEXT("Material'/Game/Assets/Materials/yellowMat.yellowMat'"));
	UMaterial* settingMaterial_s = (UMaterial*)Material_s.Object;
	selectedMat = UMaterialInstanceDynamic::Create(settingMaterial_s, NULL);

	myScale = FVector(1.0f, 1.0f, 1.0f);
}

// Called when the game starts or when spawned
void APlaceableActor::BeginPlay()
{
	Super::BeginPlay();

	//for (TObjectIterator<ACustomARPawn> It; It; ++It)
	//{
	//	pawnRef = *It;
	//}

}

// Called every frame
void APlaceableActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	bool debugProximity = false;
	if (debugProximity) {
		//distance code
		APlayerCameraManager* manRef = GetWorld()->GetFirstPlayerController()->PlayerCameraManager;
		FVector distanceToPawn = this->GetActorLocation() - manRef->GetActorTransform().GetLocation();

		if (distanceToPawn.Length() < 100 && nearby == false) {
			StaticMeshComponent->SetMaterial(0, onMat);

			nearby = true;
			//GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, TEXT("came near!"));
		}
		else if (distanceToPawn.Length() > 100 && nearby == true) {
			StaticMeshComponent->SetMaterial(0, offMat);

			nearby = false;
			//GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, TEXT("is far now!"));
		}

	}


	// Making sure the actor remains on the ARPin that has been found.
	if(PinComponent)
	{
		auto TrackingState = PinComponent->GetTrackingState();
		
		switch (TrackingState)
		{
		case EARTrackingState::Tracking:
			SceneComponent->SetVisibility(true);
			SetActorTransform(PinComponent->GetLocalToWorldTransform());

			// Scale down default cube mesh - Change this for your applications.
			//SetActorScale3D(FVector(0.2f, 0.2f, 0.2f));
			break;

		case EARTrackingState::NotTracking:
			PinComponent = nullptr;

			break;

		}
	}
	
	if (selected) {
		float x_scale = myScale.X;



		if (x_scale > 1.4f && expander > 0) {
			expander *= -1.0f;
		}
		if (x_scale < 0.9f && expander < 0) {
			expander *= -1.0f;
		}

		float dtScale = expander * DeltaTime;
		myScale += FVector(dtScale, dtScale, dtScale);
		SetActorScale3D(myScale);
		myPos.Z = PinComponent->GetLocalToWorldTransform().GetLocation().Z;
		SetActorLocation(myPos);

	}

}

