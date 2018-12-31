// Fill out your copyright notice in the Description page of Project Settings.

#include "MeshActor.h"

AMeshActor::AMeshActor()
{
	RootComponent->SetMobility(EComponentMobility::Movable);
	proceduralMesh = CreateDefaultSubobject<URuntimeMeshComponent>(TEXT("ProceduralMesh"));
	proceduralMesh->SetupAttachment(RootComponent);
	bSimulatePhysics = true;
}
//
//bool AMeshActor::PickUp()
//{
//	UE_LOG(LogTemp, Warning, TEXT("Reacting to grabbing"));
//	return false;
//}
//
//bool AMeshActor::Drop()
//{
//	UE_LOG(LogTemp, Warning, TEXT("Reacting to drop"));
//	return false;
//}

bool AMeshActor::PickUp_Implementation(USceneComponent* attachTo)
{
	UE_LOG(LogTemp, Warning, TEXT("Reacting to PickUp"));
	if (!bIsImportedMesh) {
		GetStaticMeshComponent()->SetSimulatePhysics(false);
		RootComponent->AttachToComponent(attachTo, FAttachmentTransformRules::KeepWorldTransform);
	}
	else {
		proceduralMesh->SetSimulatePhysics(false);
		proceduralMesh->AttachToComponent(attachTo, FAttachmentTransformRules::KeepWorldTransform);
	}
	return true;
}

bool AMeshActor::Drop_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("Reacting to Drop"));
	if (!bIsImportedMesh) {
		GetStaticMeshComponent()->SetSimulatePhysics(bSimulatePhysics);
		RootComponent->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	}
	else {
		proceduralMesh->SetSimulatePhysics(bSimulatePhysics);
		proceduralMesh->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	}
	
	return true;
}

