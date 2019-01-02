// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Components/SceneComponent.h"
#include "InteractionInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(BlueprintType)
class UInteractionInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class DIPLOMAVR_API IInteractionInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	int32 test;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Grab Reaction")
	bool PickUp(USceneComponent* attachTo);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Grab Reaction")
	bool PickUpByHandle(UPhysicsHandleComponent* grabHandle);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Grab Reaction")
	bool Drop();

};
