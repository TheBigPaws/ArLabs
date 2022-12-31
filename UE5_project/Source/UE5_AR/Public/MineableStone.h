// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Resource.h"
#include "MineableStone.generated.h"

//Child class of AResource. It defines a single pickupable ore 
//this ore can be made of the following types; Stone, Iron and Mithril
UCLASS()
class UE5_AR_API AMineableStone : public AResource
{
	GENERATED_BODY()
public:
	// Sets default values for this actor's properties
	AMineableStone();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	//ore vein type logic
	UPROPERTY(Category = "UIrelevant", EditAnywhere, BlueprintReadWrite)
	FString oreVein;
	void setVein(FString veinType);

};
