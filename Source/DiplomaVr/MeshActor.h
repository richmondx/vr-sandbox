// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <Engine/StaticMeshActor.h>
#include "InteractionInterface.h"
#include <Components/MeshComponent.h>
#include <Classes/PhysicsEngine/PhysicsHandleComponent.h>
#include "RuntimeMeshComponent.h"
#include "MeshActor.generated.h"

/**
 * 
 */
UCLASS()
class DIPLOMAVR_API AMeshActor : public AStaticMeshActor, public IInteractionInterface
{
	GENERATED_BODY()
	
public:

	AMeshActor();

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, meta = (DisplayName = "Is Mesh Spawned?"))
	bool bIsSpawned;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, meta = (DisplayName = "Simulate Physics?"))
	bool bSimulatePhysics;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, meta = (DisplayName = "Procedural Mesh"))
	URuntimeMeshComponent* proceduralMesh;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, meta = (DisplayName = "Is Mesh imported?"))
	bool bIsImportedMesh;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Grab Reaction")
	bool PickUp(USceneComponent* attachTo);
	virtual bool PickUp_Implementation(USceneComponent* attachTo) override;

	bool PickUpByHandle(UPhysicsHandleComponent* grabHandle);
	virtual bool PickUpByHandle_Implementation(UPhysicsHandleComponent* grabHandle) override;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Grab Reaction")
	bool Drop();
	virtual bool Drop_Implementation() override;
};
