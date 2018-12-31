// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "FriVR_GameInstance.generated.h"

/**
 * 
 */
UCLASS()
class DIPLOMAVR_API UFriVR_GameInstance : public UGameInstance
{
	GENERATED_BODY()

public: 
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Assimp Asset Path")
	FString AssetPath;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Map Preferences")
	bool bSpawnPlayer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Map Preferences")
	bool bIsVr;

	UFUNCTION(BlueprintCallable, Category = "Map Preferences")
	void SpawnPawn(UClass* VrPawn, UClass* NormalPawn, FTransform SpawnTransform);
};
