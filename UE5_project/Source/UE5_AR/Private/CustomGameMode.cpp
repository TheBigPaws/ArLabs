// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomGameMode.h"
#include "StencilBox.h"
#include "CustomARPawn.h"
#include "ARBlueprintLibrary.h"
#include "ButterflyBB.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"

ACustomGameMode::ACustomGameMode()
{
	// Add this line to your code if you wish to use the Tick() function
	PrimaryActorTick.bCanEverTick = true;

	// Set the default pawn and gamestate to be our custom pawn and gamestate programatically
	DefaultPawnClass = ACustomARPawn::StaticClass();
	GameStateClass = AGameModeBase::StaticClass();

	
}

void ACustomGameMode::StartPlay() 
{

	// This function will transcend to call BeginPlay on all the actors 
	Super::StartPlay();

	//spawn AR manager and assign initial spawn density
	Manager = GetWorld()->SpawnActor<AHelloARManager>();
	Manager->SpawnDistance = spawnDensity;
	
	//assign a reference to the player
	AActor* pawnRef_A = UGameplayStatics::GetActorOfClass(GetWorld(), ACustomARPawn::StaticClass());
	pawnRef = Cast<ACustomARPawn>(pawnRef_A);

	// visual: spawn 30 billboarded butterflies :)
	const FActorSpawnParameters butterfly_SI;
	for (int i = 0; i < 30; i++) {

		FVector randomPos = FVector(FMath::FRandRange(-500.0f, 500.0f), FMath::FRandRange(-500.0f, 500.0f), FMath::FRandRange(100.0f, 300.0f));
		AButterflyBB* butterflyRef = GetWorld()->SpawnActor<AButterflyBB>(randomPos, FRotator(0, 0, 0), butterfly_SI);

		butterflyRef->camRef = pawnRef->CameraComponent;
		

	}

}

void ACustomGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	//search for possible resource spawning positions
	update_Spawn_Positions();

	//spawn resources
	handleResourceSpawning(DeltaSeconds);

}

//adds spawn positions to the pool array, provided they are original and exist on an AR plane
void ACustomGameMode::update_Spawn_Positions() {

	if (pawnRef) { //nullpointer failsafe

		//get the gridded centre position of the spawn function
		FIntVector2 PPos_now = FIntVector2(pawnRef->CameraComponent->GetComponentLocation().X, pawnRef->CameraComponent->GetComponentLocation().Y);
		PPos_now = gridCutoff(PPos_now, spawnDensity);


		//stops continuously searching in place
		if (PPos_now != last_searchPos) {

			//set this to bee last searched pos
			last_searchPos = PPos_now;

			//get all nearby grid positions based on spawn distance and density
			TArray<FIntVector2> possibleSpawnPos = getAllNearbyGridPositions(spawnDensity, spawnDistance, PPos_now);

			//loop through these positions
			for (int j = 0; j < possibleSpawnPos.Num(); j++) {

				//see if this grid position exists on any AR plane and if so, obtain the Z location
				possible_AR_pos_return ret_ = Manager->griXYExistsOnAPlane(possibleSpawnPos[j]);

				//if the locatio is valid
				if (ret_.exists) {

					//create a vector with the Z assigned
					FVector Vector_withZ = FVector(possibleSpawnPos[j].X, possibleSpawnPos[j].Y, ret_.Zpos);

					//assume the position is original
					bool alreadyUsed = false;

					//loop through all the used and available positions and see if this one is original
					for (int l = 0; l < spawn_pos_AVAILABle.Num(); l++) {
						if (spawn_pos_AVAILABle[l].X == Vector_withZ.X && spawn_pos_AVAILABle[l].Y == Vector_withZ.Y) {
							alreadyUsed = true;
							break;
						}
					}
					if (!alreadyUsed) {
						for (int p = 0; p < assigned_spawn_pos.Num(); p++) {
							if (assigned_spawn_pos[p].X == Vector_withZ.X && assigned_spawn_pos[p].Y == Vector_withZ.Y) {
								alreadyUsed = true;
								break;
							}
						}
					}

					//if it's original, add this position into the list of available spawn positions
					if (!alreadyUsed) {
						spawn_pos_AVAILABle.Add(Vector_withZ);
					}
				}

			}
		}
	}


}

//if there are any available places to spawn materials, do so. Currently limited to 1 spawn per 0.2 seconds 
void ACustomGameMode::handleResourceSpawning(float dt) {

	//decrement spawn timer
	if (spawnTimer > 0.0f)
	{
		spawnTimer -= dt;
		return;
	}

	//if there are any available vertical spawn places, spawn there first
	if (Manager->validSpawnPlacesVERT.Num() > 0) {

		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Purple, TEXT("spawned WallSB"));

		//spawn the easter egg rainbow stencil box on the wall and apply 
		const FActorSpawnParameters SpawnInfo;
		AStencilBox* SBREF_ = GetWorld()->SpawnActor<AStencilBox>(Manager->validSpawnPlacesVERT[0].world_Position - 25 * Manager->validSpawnPlacesVERT[0].world_UP_Vector, FRotator(0, FMath::FRandRange(-180.0f, 180.0f), 0), SpawnInfo);
		SBREF_->SetActorRotation(Manager->validSpawnPlacesVERT[0].world_UP_Vector.Rotation());
		SBREF_->SetActorScale3D(FVector(0.5f, 0.5f, 0.5f));
		SBREF_->addDoor(0, 0, 0, FString("Rainbow"));

		//transfer the spawning position from available to used
		Manager->valid_USED_SpawnPlacesVERT.Add(Manager->validSpawnPlacesVERT[0].world_Position);
		Manager->validSpawnPlacesVERT.RemoveAt(0);
		spawnTimer = 0.2f;
		return;
	}

	//if any horizontal spawn positions are available, spawn a random resource there
	if (spawn_pos_AVAILABle.Num() > 0) {

		//spawn random resource
		spawnRandomResource(spawn_pos_AVAILABle[0]);

		//transfer the spawning position from available to used
		assigned_spawn_pos.Add(spawn_pos_AVAILABle[0]);
		spawn_pos_AVAILABle.RemoveAt(0);
		spawnTimer = 0.2f;
	}

}

//given a location (This assumes HORIZONTAL positions), spawn a random resource
void ACustomGameMode::spawnRandomResource(FVector s_position)
{

	

	//offcenter each spawn slightly so the world looks less robotic and griddy
	FVector Unevenness_disp_V;
	float dispRange = (float)spawnDensity / 3.5f;
	Unevenness_disp_V = FVector(FMath::FRandRange(-dispRange, dispRange), FMath::FRandRange(-dispRange, dispRange), 0);
	s_position += Unevenness_disp_V;

	//used to determine what spawns (each resource has a range)
	int randomChooser = FMath::RandRange(0, 100);
	const FActorSpawnParameters SpawnInfo;

	//determine spawn
	if (randomChooser <= 40) { //Spawn a tree random of 3 tiers, see ATree(), 40% likely
		ATree* TreeRef = GetWorld()->SpawnActor<ATree>(s_position, FRotator(0, FMath::FRandRange(-180.0f, 180.0f), 0), SpawnInfo);
	}
	else if (randomChooser <= 60) { //Spawn 2-4 stones, 20% likely
		int stonesToSpawn = FMath::RandRange(2, 4); 
		for (int i = 0; i < stonesToSpawn; i++) {
			
			FVector dispV = FVector(FMath::FRandRange(-80.0f, 80.0f), FMath::FRandRange(-80.0f, 80.0f), 0);

			AMineableStone* stoneREF = GetWorld()->SpawnActor<AMineableStone>(s_position + dispV, FRotator(0, FMath::FRandRange(-180.0f, 180.0f), 0), SpawnInfo);
			stoneREF->SetActorScale3D(FVector(0.07f, 0.07f, 0.07f));
			stoneREF->setVein(FString("Stone"));
		}

	}
	else if (randomChooser <= 90) { //Spawn an iron deposit (64 health) with 4-8 ores, 30% likely
		AStencilBox* SBREF_2 = GetWorld()->SpawnActor<AStencilBox>(s_position - FVector(0, 0, 25), FRotator(0, FMath::FRandRange(-180.0f, 180.0f), 0), SpawnInfo);
		SBREF_2->SetActorScale3D(FVector(0.5f, 0.5f, 0.5f));
		SBREF_2->SetActorRotation(FVector(0, 0, 1).Rotation());
		SBREF_2->addDoor(1, 8, FMath::RandRange(4,8), FString("Iron"));

	}
	else if (randomChooser <= 100) { //Spawn a Mithril deposit (225 health) with 3-6 ores, 10% likely
		AStencilBox* SBREF_3 = GetWorld()->SpawnActor<AStencilBox>(s_position - FVector(0, 0, 25), FRotator(0, FMath::FRandRange(-180.0f, 180.0f), 0), SpawnInfo);
		SBREF_3->SetActorScale3D(FVector(0.5f, 0.5f, 0.5f));
		SBREF_3->SetActorRotation(FVector(0, 0, 1).Rotation());
		SBREF_3->addDoor(1, 15, FMath::RandRange(3, 6), FString("Mithril"));
	}

}

//returns what position on the grid this would be treated as (always lower X and Y)
FIntVector2 ACustomGameMode::gridCutoff(FIntVector2 pos_, int32 gridSize) {

	FIntVector2 pos = pos_;
	pos.X = (pos.X / gridSize) * gridSize;
	pos.Y = (pos.Y / gridSize) * gridSize;

	return pos;
}

//returns nearby locations on an imaginary grid specified by function parameters
TArray<FIntVector2> ACustomGameMode::getAllNearbyGridPositions(int32 spawnDensity_, int32 spawnDistance_, FIntVector2 middlePos_)
{

	//declare return
	TArray<FIntVector2> return_positions;

	//define the extent of the grid
	FIntVector2 grid_up_left_pos = gridCutoff(FIntVector2(middlePos_.X - spawnDistance_, middlePos_.Y - spawnDistance_), spawnDensity_);
	FIntVector2 dow_right_pos = FIntVector2(middlePos_.X + spawnDistance_, middlePos_.Y + spawnDistance_);

	//define steps
	int32 steps_X = 0;
	int32 steps_Y = 0;

	//loop through the grid using the density as spacers and add the positions into the return array
	bool stillSearching = true;
	while (stillSearching) {

		//add this iterated position to the return array
		FIntVector2 it_Pos = grid_up_left_pos;
		it_Pos.X += steps_X * spawnDensity_;
		it_Pos.Y += steps_Y * spawnDensity_;
		return_positions.Add(it_Pos);


		//iterate offsets and exit loop condition
		steps_X++;
		if (it_Pos.X + spawnDensity_ > dow_right_pos.X) {
			steps_X = 0;
			steps_Y++;
		}
		if (it_Pos.Y + spawnDensity_ > dow_right_pos.Y && it_Pos.X + spawnDensity_ > dow_right_pos.X) {
			stillSearching = false;
		}
	}


	return return_positions;
}

//Interact mode setter, used by the UI
void ACustomGameMode::setInteractMode(FString setter)
{
	interactionState = setter;
}

//spawn density setting function (used by the UI)
void ACustomGameMode::setSpawnDensity(int32 newSpawnDensity)
{
	spawnDensity = newSpawnDensity;
	spawnDistance = spawnDensity * 2 + 5;
	Manager->SpawnDistance = spawnDensity;
}

//Main ray function from a phone screen tap function
bool ACustomGameMode::WorldHitTest(FVector2D screenTouchPos, FHitResult& hitResult) {

	// Get player controller
	APlayerController* playerController = UGameplayStatics::GetPlayerController(this, 0);
	if (!playerController) {
		return false;
	}



	// Perform deprojection taking 2d clicked area and generating reference in 3d world-space.
	FVector worldPosition;
	FVector worldDirection; // Unit Vector

	bool deprojectionSuccess = UGameplayStatics::DeprojectScreenToWorld(playerController, screenTouchPos, /*out*/
		worldPosition, /*out*/ worldDirection);

	if (deprojectionSuccess) {
		// construct trace vector (from point clicked to 1000.0 units beyond in same direction)
		FVector traceEndVector = worldDirection * 10000.0;
		traceEndVector = worldPosition + traceEndVector;
		// perform line trace (Raycast)
		bool traceSuccess = GetWorld()->LineTraceSingleByChannel(hitResult, worldPosition, traceEndVector, ECollisionChannel::ECC_WorldDynamic);
		// return if the operation was successful
		return traceSuccess;
	}
	return false;

}
