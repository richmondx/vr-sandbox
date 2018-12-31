// Fill out your copyright notice in the Description page of Project Settings.

#include "FriVR_GameInstance.h"

void UFriVR_GameInstance::SpawnPawn(UClass* VrPawn, UClass* NormalPawn, FTransform SpawnTransform)
{
	if (bSpawnPlayer)
	{

	APlayerController* playerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	// playerController->UnPossess();
	if (playerController->GetPawn())
	{
		UE_LOG(LogTemp, Warning, TEXT("Remove default pawn:%s\n"), *(playerController->GetPawn()->GetHumanReadableName()));
		playerController->GetPawn()->Destroy();
	}
	APawn* PlayerPawn;

	FActorSpawnParameters SpawnInfo;
	SpawnInfo.bNoFail = true;
	SpawnInfo.bDeferConstruction = false;

	if (bIsVr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Spawning in VR Mode\n"));
		SpawnInfo.Name = "PlayerPawn_VR";
		PlayerPawn = Cast<APawn>(GetWorld()->SpawnActor(VrPawn, &SpawnTransform, SpawnInfo));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Spawning in Normal Mode\n"));
		SpawnInfo.Name = "PlayerPawn_Normal";
		PlayerPawn = Cast<APawn>(GetWorld()->SpawnActor(NormalPawn, &SpawnTransform, SpawnInfo));
	}

	
	playerController->Possess(PlayerPawn);
	
	}
}
