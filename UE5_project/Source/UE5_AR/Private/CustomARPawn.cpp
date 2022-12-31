// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomARPawn.h"
#include "Kismet/KismetMathLibrary.h"
#include "Camera/CameraComponent.h"
#include "Engine/EngineTypes.h"
#include "CubeCrumbling.h"
#include "ShopPREVIEW.h"
#include "Shop.h"
#include "OpeningDoor.h"
#include "ARPlaneActor.h"
#include "TextActor.h"
#include "MineableStone.h"

// Sets default values
ACustomARPawn::ACustomARPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	SetRootComponent(SceneComponent);

	//create and attach camera component
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetupAttachment(SceneComponent);
	

	//find and assign player interaction sounds
	static ConstructorHelpers::FObjectFinder<USoundWave> TreeHit_AudiRes(TEXT("SoundWave'/Game/Audio/TreeHit_.TreeHit_'"));
	static ConstructorHelpers::FObjectFinder<USoundWave> shopUpgrade_AudiRes(TEXT("SoundWave'/Game/Audio/win_sound_2-3.win_sound_2-3'"));
	static ConstructorHelpers::FObjectFinder<USoundWave> pickupSound_AudiRes(TEXT("SoundWave'/Game/Audio/item_pickup.item_pickup'"));
	static ConstructorHelpers::FObjectFinder<USoundWave> miningSound_AudiRes(TEXT("SoundWave'/Game/Audio/PickaxeOnStone_.PickaxeOnStone_'"));
	static ConstructorHelpers::FObjectFinder<USoundWave> TreeFallngSound_AudiRes(TEXT("SoundWave'/Game/Audio/multiple_cracks_1.multiple_cracks_1'"));

	if (TreeHit_AudiRes.Succeeded()) {
		treeChopSound = TreeHit_AudiRes.Object;
	}
	if (shopUpgrade_AudiRes.Succeeded()) {
		shopUpgrade = shopUpgrade_AudiRes.Object;
	}
	if (pickupSound_AudiRes.Succeeded()) {
		pickupSound = pickupSound_AudiRes.Object;
	}
	if (miningSound_AudiRes.Succeeded()) {
		miningSound = miningSound_AudiRes.Object;
	}
	if (TreeFallngSound_AudiRes.Succeeded()) {
		TreeFallngSound = TreeFallngSound_AudiRes.Object;
	}
}

// Called when the game starts or when spawned
void ACustomARPawn::BeginPlay()
{
	Super::BeginPlay();

	//get a reference to GameMode and assign it
	auto Temp = GetWorld()->GetAuthGameMode();
	GM_ref = Cast<ACustomGameMode>(Temp);

}


// Called every frame
void ACustomARPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	
	if (buildPreview) {
		moveShopPreview();
	}
}

// Called to bind functionality to input
void ACustomARPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);


	//Bind various player inputs to functions
	PlayerInputComponent->BindTouch(IE_Pressed, this, &ACustomARPawn::OnScreenTouch);
	PlayerInputComponent->BindTouch(IE_Repeat, this, &ACustomARPawn::OnScreenHeld);

}

void ACustomARPawn::OnScreenHeld(const ETouchIndex::Type FingerIndex, const FVector ScreenPos) {

	//exit if the game mode was improperly assigned
	if (!GM_ref) {
		return;
	}

	//If the user is building and sliding their finger, apply rotation
	if (GM_ref->interactionState == FString("Build") && buildPreview) {

		float rot_ = ScreenPos.X - initScreenXforRotate;
		buildPreview->SetActorRotation(FRotator(0, rot_, 0));
	}

	//execute a world hit
	FHitResult HRRef;
	if (!GM_ref->WorldHitTest(FVector2D(ScreenPos), HRRef))
	{
		return;
	}
	
	//With this, the player is also able to slide their finger across the screen to collect resources
	if (GM_ref->interactionState == FString("Interact")) {

		//if the object is too far, don't interact with it
		if ((CameraComponent->GetComponentLocation() - HRRef.Location).Length() >= interactionReach) { //distance based interaction
			return;
		}

		//casts of the object to different classes, incrementing into the correct inventory variable and destroying the material
		ALogPickup* LogPicupRef = Cast<ALogPickup>(HRRef.GetActor());
		if (LogPicupRef) {
			UGameplayStatics::PlaySound2D(this, pickupSound);
			Wood_In_Inventory++;
			LogPicupRef->Destroy();
			return;
		}
		ALogPile* LogPILE_Ref = Cast<ALogPile>(HRRef.GetActor());
		if (LogPILE_Ref) {
			UGameplayStatics::PlaySound2D(this, pickupSound);
			Wood_In_Inventory += 14;
			LogPILE_Ref->Destroy();
			return;
		}
		AMineableStone* StoneRef = Cast<AMineableStone>(HRRef.GetActor());
		if (StoneRef) {
			UGameplayStatics::PlaySound2D(this, pickupSound);

			if (StoneRef->oreVein == FString("Stone")) { //determine the ore type
				Stone_In_Inventory++;
			}else if (StoneRef->oreVein == FString("Iron")) {
				Iron_In_Inventory++;
			}else if (StoneRef->oreVein == FString("Mithril")) {
				Mithril_In_Inventory++;
			}
			StoneRef->Destroy();
		}
	}

}

//most of logic is in this function as it is bound to touch on the screen. Interactions are filtered by the current interaction state
void ACustomARPawn::OnScreenTouch(const ETouchIndex::Type FingerIndex, const FVector ScreenPos)
{

	//assign the first X of a touch - usex to rotate build
	initScreenXforRotate = ScreenPos.X;


	// Perform a hitTest, get the return values as hitTesTResult
	FHitResult HRRef;
	if (!GM_ref->WorldHitTest(FVector2D(ScreenPos), HRRef))
	{
		return;
	}

	//state based interactions
	if (GM_ref->interactionState == FString("Interact")) {

		//if the object was out of reach, get out. there is no reach on Examine or build
		if ((CameraComponent->GetComponentLocation() - HRRef.Location).Length() >= interactionReach) {
			return;
		}


		//For mining, decide whether it was a critical hit (1 in 10 chance)
		int critMulp = FMath::RandRange(0, 10);
		bool isCrit = true;
		if (critMulp == 0) {
			critMulp = 3;
			isCrit = true;
		}
		else {
			critMulp = 1;
			isCrit = false;
		}

		//CASTS to filter interactions
		ACreumbleWallPiece* crumblCubeRef = Cast<ACreumbleWallPiece>(HRRef.GetActor());
		if (crumblCubeRef) {
			
			//apply damage to CrumbleWall, play sound and spawn damage text
			UGameplayStatics::PlaySound2D(this, miningSound);
			spawnDamageTextAtPos(isCrit, miningStrength * critMulp, HRRef.GetActor()->GetActorLocation());
			crumblCubeRef->parentRef->partlyCrumble(miningStrength * critMulp);

			return;
		}
		AOpeningDoor* openingRef = Cast<AOpeningDoor>(HRRef.GetActor());
		if (openingRef) {
			if (openingRef->openProgress == 0.0f && !openingRef->opening) {

				//set both doors to open
				openingRef->opening = true;
				openingRef->otherDoor->opening = true;

				//spawn prize inside the special rainbow box.... nothing. 
				const FActorSpawnParameters SpawnInfo_OD;
				ATextActor* BillBoardedTXT = GetWorld()->SpawnActor<ATextActor>(openingRef->stencilBoxRef->GetActorLocation() - FVector(0,0,15), FRotator(0, 0, 0), SpawnInfo_OD);
				BillBoardedTXT->setupText("WOW, YOU GOT\n.........\nnothing :D", 30.0f, FColor::Black, 8.0f, CameraComponent, FVector(0, 0, 0));
			}
			return;
		}
		ATree* TreeRef = Cast<ATree>(HRRef.GetActor());
		if (TreeRef) {

			//don't interact with a tree that's already falling
			if (TreeRef->state_ != FString("Falling")) {

				//deal damage to tree, spawn damage text and play sound
				TreeRef->RSShealth -= miningStrength * critMulp;
				spawnDamageTextAtPos(isCrit, miningStrength * critMulp, HRRef.Location);
				UGameplayStatics::PlaySound2D(this, treeChopSound); 

				//if the tree does not have a health text attached to it yet (if it's never been hit), attach one
				if (TreeRef->healthTextRef == nullptr) {
					const FActorSpawnParameters SpawnInfo;
					ATextActor* treeHP_TXT = GetWorld()->SpawnActor<ATextActor>(HRRef.GetActor()->GetActorLocation() + FVector(0,0,HRRef.GetActor()->GetComponentsBoundingBox().GetExtent().Z/3.0f) + (CameraComponent->GetComponentLocation() - HRRef.Location).GetSafeNormal() * 50, FRotator(0, 0, 0), SpawnInfo);
					treeHP_TXT->setupText(FString("Health: ").Append(FString::FromInt(TreeRef->RSShealth)), 1.0f, FColor::White, 20.0f, CameraComponent);
					treeHP_TXT->timeDestroyable = false;
					TreeRef->healthTextRef = treeHP_TXT;
				}

				//tree is felled, play sound and start falling animation
				if(TreeRef->RSShealth <= 0)
				{
					TreeRef->state_ = FString("Falling");
					UGameplayStatics::PlaySound2D(this, TreeFallngSound); 
				}else
				{
					//update tree health text and initiate small tree shaking animation upon hit
					TreeRef->state_ = FString("ShakingOUT");
					TreeRef->SetActorRotation(FRotator(0,0,0));
					TreeRef->stateIncrementor = 0.0f;
					TreeRef->shakingDesiredRPY = FVector(FMath::FRandRange(-10.0f,10.0f), FMath::FRandRange(-10.0f, 10.0f), FMath::FRandRange(-10.0f, 10.0f));
					TreeRef->healthTextRef->myTextComponent->SetText(FText::FromString(FString("Health: ").Append(FString::FromInt(TreeRef->RSShealth))));
				}
			}
			return;
		}
		AShop* shopREF = Cast<AShop>(HRRef.GetActor()); 
		if (shopREF) {

			//if the player is able to upgrade their tool
			bool succesfullyUpgraded = shopREF->CanUpgradeTool(Wood_In_Inventory,Stone_In_Inventory,Iron_In_Inventory,Mithril_In_Inventory);
			if (succesfullyUpgraded) {
				switch (shopREF->upgradeTier) //depending on the tier of the upgrade, make the mining strenght better
				{
				case 1:
					miningStrength = 3;
					currentToolLevel = FString("Stone");
					break;
				case 2:
					miningStrength = 10;
					currentToolLevel = FString("Iron");
					break;
				case 3:
					miningStrength = 20;
					currentToolLevel = FString("Mithril");
					break;
				case 4:
					miningStrength = 50;
					break;

				}

				//pay for the upgrade
				Wood_In_Inventory -= shopREF->Wood_COST;
				Stone_In_Inventory -= shopREF->Stone_COST;
				Iron_In_Inventory -= shopREF->Iron_COST;
				Mithril_In_Inventory -= shopREF->Mithril_COST;

				//tier up the upgrade in the shop and play purchase sound
				shopREF->TierUp();
				UGameplayStatics::PlaySound2D(this, shopUpgrade);

			}
			else {

				//prepare upgrade string
				FString pricesTEXT = FString("I don't have enough materials!\nI need:");
				pricesTEXT.Append(FString("\nWood: "));
				pricesTEXT.Append(FString::FromInt(shopREF->Wood_COST));
				pricesTEXT.Append(FString("\nStone: "));
				pricesTEXT.Append(FString::FromInt(shopREF->Stone_COST));
				pricesTEXT.Append(FString("\nIron: "));
				pricesTEXT.Append(FString::FromInt(shopREF->Iron_COST));
				pricesTEXT.Append(FString("\nMithril: "));
				pricesTEXT.Append(FString::FromInt(shopREF->Mithril_COST));

				//calculate position vector so that the text will display in front of the store, in the middle
				FVector spawnPos_txxt = HRRef.GetActor()->GetActorLocation();
				spawnPos_txxt += (HRRef.GetActor()->GetComponentsBoundingBox().GetExtent().Y + 20) * HRRef.GetActor()->GetActorForwardVector();
				spawnPos_txxt.Z += HRRef.GetActor()->GetComponentsBoundingBox().GetExtent().Z;

				//spawn the text and assign its position
				const FActorSpawnParameters SpawnInfo;
				ATextActor* BillBoardedTXT = GetWorld()->SpawnActor<ATextActor>(spawnPos_txxt, FRotator(0, 0, 0), SpawnInfo);
				BillBoardedTXT->setupText(pricesTEXT, 3.0f, FColor::Yellow, 10.0f, nullptr, FVector(0, 0, 0));
				BillBoardedTXT->myTextComponent->SetRelativeRotation(HRRef.GetActor()->GetActorForwardVector().Rotation());


			}
		}
	}
	else if (GM_ref->interactionState == FString("Build")) {
		AShopPREVIEW* shopPRE = Cast<AShopPREVIEW>(HRRef.GetActor()); 
		if (shopPRE) {

			//pressed on the preview so replace it by the actual structure
			const FActorSpawnParameters SpawnInfo;
			AShop* shop_ref = GetWorld()->SpawnActor<AShop>(FVector(0,0,0), FRotator(0, FMath::FRandRange(-180.0f, 180.0f), 0), SpawnInfo);
			shop_ref->SetActorTransform(shopPRE->GetActorTransform());

			//destroy the preview, pay for the build, play build sound and set mode to interact
			destroyShopPreview();
			Wood_In_Inventory -= 5;
			GM_ref->setInteractMode(FString("Interact"));
			UGameplayStatics::PlaySound2D(this, treeChopSound);
		}
	}
	else if (GM_ref->interactionState == FString("Examine")) {

		const FActorSpawnParameters SpawnInfo;

		//show a helpful message to the player when they Examine objects
		ACreumbleWallPiece* crumblCubeRef = Cast<ACreumbleWallPiece>(HRRef.GetActor()); //
		if (crumblCubeRef) {
			ATextActor* BillBoardedTXT = GetWorld()->SpawnActor<ATextActor>(HRRef.GetActor()->GetActorLocation() + FVector(0, 0, HRRef.GetActor()->GetComponentsBoundingBox().GetExtent().Z + 20), FRotator(0, 0, 0), SpawnInfo);
			BillBoardedTXT->setupText(FString("Hmm... Maybe there's some\n valuable ore here?"), 3.0f, FColor::Yellow, 13.0f, CameraComponent, FVector(0, 0, 5));
			return;
		}
		AShop* shopEREF = Cast<AShop>(HRRef.GetActor()); // 
		if (shopEREF) {
			ATextActor* BillBoardedTXT = GetWorld()->SpawnActor<ATextActor>(HRRef.GetActor()->GetActorLocation() + FVector(0, 0, HRRef.GetActor()->GetComponentsBoundingBox().GetExtent().Z*2 + 20), FRotator(0, 0, 0), SpawnInfo);
			BillBoardedTXT->setupText(FString("I bet I can upgrade\n my tool here!"), 3.0f, FColor::Yellow, 13.0f, CameraComponent, FVector(0, 0, 5));
			return;
		}
		ATree* TreeRef = Cast<ATree>(HRRef.GetActor()); //
		if (TreeRef) {
			ATextActor* BillBoardedTXT = GetWorld()->SpawnActor<ATextActor>(HRRef.GetActor()->GetActorLocation() + FVector(0, 0, HRRef.GetActor()->GetComponentsBoundingBox().GetExtent().Z*2 + 20), FRotator(0, 0, 0), SpawnInfo);
			BillBoardedTXT->setupText(FString("Usually people start \ngames by punching a tree..."), 3.0f, FColor::Yellow, 13.0f, CameraComponent, FVector(0, 0, 5));
			return;
		}
		ALogPickup* LogPC = Cast<ALogPickup>(HRRef.GetActor()); // 
		ALogPile* LogPILE = Cast<ALogPile>(HRRef.GetActor()); // 
		AMineableStone* STONEREF = Cast<AMineableStone>(HRRef.GetActor()); // 
		if (LogPC || LogPILE || STONEREF) {
			ATextActor* BillBoardedTXT = GetWorld()->SpawnActor<ATextActor>(HRRef.GetActor()->GetActorLocation() + FVector(0, 0, HRRef.GetActor()->GetComponentsBoundingBox().GetExtent().Z*2+20), FRotator(0, 0, 0), SpawnInfo);
			BillBoardedTXT->setupText(FString("Raw resources on \nthe floor? YOINK!"), 3.0f, FColor::Yellow, 13.0f, CameraComponent, FVector(0, 0, 5));
		}
	}
}

//function called from the UI when player starts building
void ACustomARPawn::constructShopPreview() {
	buildPreview = GetWorld()->SpawnActor(AShopPREVIEW::StaticClass());
	buildPreview->SetActorScale3D(FVector(0.5f, 0.5f, 0.5f));
	auto Temp = GetWorld()->GetAuthGameMode();
	auto GM = Cast<ACustomGameMode>(Temp);
	GM->setInteractMode(FString("Build"));
}

//function called from the UI when player cancels building
void ACustomARPawn::destroyShopPreview() {
	buildPreview->Destroy();
	buildPreview = nullptr;
	auto Temp = GetWorld()->GetAuthGameMode();
	auto GM = Cast<ACustomGameMode>(Temp);
	GM->setInteractMode(FString("Interact"));
}

//move the preview to where the player is looking
void ACustomARPawn::moveShopPreview()
{

	//cast a ray down the middle of the screen
	const FVector2D ViewportSize = FVector2D(GEngine->GameViewport->Viewport->GetSizeXY());
	auto TraceResult = UARBlueprintLibrary::LineTraceTrackedObjects(FVector2D(ViewportSize.X / 2, ViewportSize.Y / 2), false, false, false, true);

	//if it reached anything, move the shop preview there
	if (TraceResult.Num() > 0) {
		auto TrackedTF = TraceResult.Last().GetLocalToWorldTransform();
		buildPreview->SetActorLocation(TrackedTF.GetLocation());
	}
}

//function that spawns floating damage text (similar to MMOs like Runescape or Warcraft) at a given location
void ACustomARPawn::spawnDamageTextAtPos(bool isCrit, int damageTaken, FVector position) {

	const FActorSpawnParameters SpawnInfo;

	//default colour, size and direction values 
	FVector randDir = FVector(FMath::FRandRange(-15.0f, 15.0f), FMath::FRandRange(-15.0f, 15.0f), FMath::FRandRange(25.0f, 40.0f));
	FColor dispC = FColor::Yellow;
	float textSize_ = 18.0f;

	//enlarge these values if it was a crititcal hit
	if (isCrit) {
		dispC = FColor::Red;
		randDir *= 1.5f;
		textSize_ += 5.0f;
	}

	//spawn the billboarded text actor
	ATextActor* BillBoardedTXT = GetWorld()->SpawnActor<ATextActor>(position, FRotator(0, 0, 0), SpawnInfo);
	BillBoardedTXT->setupText(FString::FromInt(-damageTaken), 1.0f, dispC, textSize_, CameraComponent, randDir);
}
