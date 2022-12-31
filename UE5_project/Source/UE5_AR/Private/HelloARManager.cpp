// Fill out your copyright notice in the Description page of Project Settings.


#include "HelloARManager.h"
#include "ARPlaneActor.h"
#include "ARPin.h"
#include "ARSessionConfig.h"
#include "ARBlueprintLibrary.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AHelloARManager::AHelloARManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// This constructor helper is useful for a quick reference within UnrealEngine and if you're working alone. But if you're working in a team, this could be messy.
	// If the artist chooses to change the location of an art asset, this will throw errors and break the game.
	// Instead let unreal deal with this. Usually avoid this method of referencing.
	// For long term games, create a Data-Only blueprint (A blueprint without any script in it) and set references to the object using the blueprint editor.
	// This way, unreal will notify your artist if the asset is being used and what can be used to replace it.
	static ConstructorHelpers::FObjectFinder<UARSessionConfig> ConfigAsset(TEXT("ARSessionConfig'/Game/Blueprints/HelloARSessionConfig.HelloARSessionConfig'"));
	Config = ConfigAsset.Object;

	//developer occlusion option
	bool occlusion = true;
	if (occlusion) {
		Config->bUseSceneDepthForOcclusion = true;
		Config->SetSessionTrackingFeatureToEnable(EARSessionTrackingFeature::SceneDepth);
	}
	else {
		Config->bUseSceneDepthForOcclusion = false;
		Config->SetSessionTrackingFeatureToEnable(EARSessionTrackingFeature::None);
	}
	

	//Populate the plane colours array
	PlaneColors.Add(FColor::Blue);
	PlaneColors.Add(FColor::Red);
	PlaneColors.Add(FColor::Green);
	PlaneColors.Add(FColor::Cyan);
	PlaneColors.Add(FColor::Magenta);
	PlaneColors.Add(FColor::Emerald);
	PlaneColors.Add(FColor::Orange);
	PlaneColors.Add(FColor::Purple);
	PlaneColors.Add(FColor::Turquoise);
	PlaneColors.Add(FColor::White);
	PlaneColors.Add(FColor::Yellow);
}



// Called when the game starts or when spawned
void AHelloARManager::BeginPlay()
{
	Super::BeginPlay();

	//Start the AR Session
	UARBlueprintLibrary::StartARSession(Config);

}



// Called every frame
void AHelloARManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	switch (UARBlueprintLibrary::GetARSessionStatus().Status)
	{
	case EARSessionStatus::Running:
		
		UpdatePlaneActors();

		//count down towards next check for overlapped planes
		combinePlanesTimer -= DeltaTime;
		if (combinePlanesTimer <= 0.0f) {
			bool stillCombining = true;
			while (stillCombining) {
				stillCombining = combinePlanes();
			}

			combinePlanesTimer = 5.0f;
		}

		break;

	case EARSessionStatus::FatalError:

		ResetARCoreSession();
		UARBlueprintLibrary::StartARSession(Config);
		break;
	}
}


//function that consumes one overlapping plane, if there is one
//can be run in the tick, however it performs better when called
//occasionally (currently every 5 seconds) in a while loop, consuming all overlapped planes
bool AHelloARManager::combinePlanes()
{
	

	for (const TPair<UARPlaneGeometry*, AARPlaneActor*>& pair : PlaneActors)
	{
		for (const TPair<UARPlaneGeometry*, AARPlaneActor*>& pair_2 : PlaneActors)
		{


			if (abs(pair.Value->ARCorePlaneObject->GetLocalToTrackingTransform().GetLocation().Z - pair_2.Value->ARCorePlaneObject->GetLocalToTrackingTransform().GetLocation().Z) < 15 && pair != pair_2) {

				//record central positions of 2 planes
				float planeAposX = pair.Value->ARCorePlaneObject->GetLocalToTrackingTransform().GetLocation().X;
				float planeAposY = pair.Value->ARCorePlaneObject->GetLocalToTrackingTransform().GetLocation().Y;
				float planeBposX = pair_2.Value->ARCorePlaneObject->GetLocalToTrackingTransform().GetLocation().X;
				float planeBposY = pair_2.Value->ARCorePlaneObject->GetLocalToTrackingTransform().GetLocation().Y;

				//record extents of 2 planes
				FVector extentA = pair.Value->ARCorePlaneObject->GetExtent();
				FVector extentB = pair_2.Value->ARCorePlaneObject->GetExtent();

				//see if plane B is inside fully by plane A. If so, destroy it, as it is unnecessary performance strain
				if (planeAposX + extentA.X >= planeBposX + extentB.X && planeAposX - extentA.X <= planeBposX - extentB.X && planeAposY + extentA.Y >= planeBposY + extentB.Y && planeAposY - extentA.Y <= planeBposY - extentB.Y) {// if the planes overlap on XY axis

					//  note it is important in which order objects get destroyed/removed
					//  from a list, as well as UARPlaneGeometry requiring
					//  the ConditionalBeginDestroy function, otherwise the application will crash

					pair_2.Value->Destroy();
					UARPlaneGeometry* ref_ = pair_2.Key;
					PlaneActors.Remove(ref_);
					ref_->ConditionalBeginDestroy();

					return true;
				}
			}
		}
	}

	return false;
}

//given an XY position, this function will return information about whether this location
//exists on an AR plane. If it does, it will also return the Z position of this location
possible_AR_pos_return AHelloARManager::griXYExistsOnAPlane(FIntVector2 position)
{
	//default return value
	possible_AR_pos_return returnValue = possible_AR_pos_return(false,0.0f);


	for (const TPair<UARPlaneGeometry*, AARPlaneActor*>& pair : PlaneActors)
	{
			//get extent about iterated plane
			float planeAposX = pair.Value->ARCorePlaneObject->GetLocalToWorldTransform().GetLocation().X;
			float planeAposY = pair.Value->ARCorePlaneObject->GetLocalToWorldTransform().GetLocation().Y;
			FVector extentA = pair.Value->ARCorePlaneObject->GetExtent();


			// if the points exists inside an AR plane, return true and Z pos
			if (planeAposX + extentA.X >= position.X && planeAposX - extentA.X <= position.X && planeAposY + extentA.Y >= position.Y && planeAposY - extentA.Y <= position.Y) {// if the planes overlap on XY axis
				
				returnValue.exists = true;
				returnValue.Zpos = pair.Value->ARCorePlaneObject->GetLocalToWorldTransform().GetLocation().Z;
				return returnValue;
			}
	}

	//return false
	return returnValue;
}


//loops through all the planes, seeing if they're vertical. If so, note the possible spawn location down
void AHelloARManager::noteValidVerticalLocs()
{

	for (const TPair<UARPlaneGeometry*, AARPlaneActor*>& pair : PlaneActors)
	{



		//Get the Up vector for this plane
		FVector planeUpV = pair.Key->GetLocalToWorldTransform().GetRotation().GetUpVector();
		planeUpV.Normalize();
		//IF THE PLANE IS VERTICAL
		if (abs(planeUpV.Z) < 0.2f) {

			//get all boundary vertices
			TArray<FVector> possibleEdgePlaces = pair.Key->GetBoundaryPolygonInLocalSpace();

			//find middle pos of plane
			FVector middlePos = FVector(0, 0, 0);
			for (int j = 0; j < possibleEdgePlaces.Num(); j++) {
				middlePos += possibleEdgePlaces[j];

			}
			middlePos /= possibleEdgePlaces.Num();

			// apply world transform
			middlePos += pair.Key->GetLocalToWorldTransform().GetLocation(); 

			//see if it is too close to a different vertical point
			bool validPosition = true;
			for (int n = 0; n < valid_USED_SpawnPlacesVERT.Num(); n++) {
				if ((middlePos - valid_USED_SpawnPlacesVERT[n]).Length() <= SpawnDistance) {
					validPosition = false;
					break;
				}
			}

			//if it's far from other vertical spawns, note its location and upVector
			if (validPosition) {
				SpawnPosAndUpV SP;

				SP.world_Position = middlePos;
				SP.world_UP_Vector = planeUpV;
				validSpawnPlacesVERT.Add(SP);
				return;
			}
		}
	}

}

//Updates the geometry actors in the world
void AHelloARManager::UpdatePlaneActors()
{
	//Get all world geometries and store in an array
	auto Geometries = UARBlueprintLibrary::GetAllGeometriesByClass<UARPlaneGeometry>();
	bool bFound = false;

	  

	//Loop through all geometries
	for (auto& It : Geometries)
	{
		//Check if current plane exists 
		if (PlaneActors.Contains(It))
		{
			AARPlaneActor* CurrentPActor = *PlaneActors.Find(It);

			//Check if plane is subsumed
			if (It->GetSubsumedBy()->IsValidLowLevel())
			{
				CurrentPActor->Destroy();
				PlaneActors.Remove(It);
				break;
			}
			else
			{
				//Get tracking state switch
				switch (It->GetTrackingState())
				{
					//If tracking update
				case EARTrackingState::Tracking:
					CurrentPActor->UpdatePlanePolygonMesh();
					break;
					//If not tracking destroy the actor and remove from map of actors
				case EARTrackingState::StoppedTracking:
					CurrentPActor->Destroy();
					PlaneActors.Remove(It);
					break;
				}
			}
		}
		else
		{
			//Get tracking state switch
			switch (It->GetTrackingState())
			{

			case EARTrackingState::Tracking:
				if (!It->GetSubsumedBy()->IsValidLowLevel())
				{
					PlaneActor = SpawnPlaneActor();
					PlaneActor->SetColor(GetPlaneColor(PlaneIndex));
					PlaneActor->ARCorePlaneObject = It;

					PlaneActors.Add(It, PlaneActor);
					PlaneActor->UpdatePlanePolygonMesh();
					PlaneIndex++;
				}
				break;
			}
		}

	}
	noteValidVerticalLocs();
}

//Simple spawn function for the tracked AR planes
AARPlaneActor* AHelloARManager::SpawnPlaneActor()
{
	const FActorSpawnParameters SpawnInfo;
	const FRotator MyRot(0, 0, 0);
	const FVector MyLoc(0, 0, 0);

	AARPlaneActor* CustomPlane = GetWorld()->SpawnActor<AARPlaneActor>(MyLoc, MyRot, SpawnInfo);

	return CustomPlane;
}

//Gets the colour to set the plane to when its spawned
FColor AHelloARManager::GetPlaneColor(int Index)
{
	return PlaneColors[Index % PlaneColors.Num()];
}

//does not actually work, refer to the resetAR() function for a more thorough approach that also gets rid of the UARPlaneGeometry objects
void AHelloARManager::ResetARCoreSession()
{

	//Get all actors in the level and destroy them as well as emptying the respective arrays
	TArray<AActor*> Planes;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AARPlaneActor::StaticClass(), Planes);

	for ( auto& It : Planes)
		It->Destroy();
	
	Planes.Empty();
	PlaneActors.Empty();

}


void AHelloARManager::resetAR() {

	bool destroying = true;

	while (destroying)
	{
		destroying = false;
		for (const TPair<UARPlaneGeometry*, AARPlaneActor*>& pair : PlaneActors)
		{
			pair.Value->Destroy();
			UARPlaneGeometry* ref_ = pair.Key;
			PlaneActors.Remove(ref_);
			ref_->ConditionalBeginDestroy();
			destroying = true;
			break;
		}
	}
	PlaneActors.Empty();

	auto Geometries = UARBlueprintLibrary::GetAllGeometriesByClass<UARPlaneGeometry>();
	for(int i = 0; i < Geometries.Num();i++){
		Geometries[i]->ConditionalBeginDestroy();
	}
	Geometries.Empty();

	UARBlueprintLibrary::StopARSession();
	UARBlueprintLibrary::StartARSession(Config);

}

