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
		GetStaticMeshComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
		RootComponent->AttachToComponent(attachTo, FAttachmentTransformRules::KeepWorldTransform);
	}
	else {
		proceduralMesh->SetSimulatePhysics(false);
		proceduralMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
		proceduralMesh->AttachToComponent(attachTo, FAttachmentTransformRules::KeepWorldTransform);
	}
	return true;
}

bool AMeshActor::PickUpByHandle_Implementation(UPhysicsHandleComponent* grabHandle)
{
	UE_LOG(LogTemp, Warning, TEXT("Reacting to PickUpcby handle"));
	if (!bIsImportedMesh) {
		GetStaticMeshComponent()->SetSimulatePhysics(true);
		GetStaticMeshComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
		grabHandle->GrabComponent(GetStaticMeshComponent(), *FString("None"), GetStaticMeshComponent()->GetComponentLocation(), true);
	}
	else {
		proceduralMesh->SetSimulatePhysics(true);
		proceduralMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
		grabHandle->GrabComponent(proceduralMesh, *FString("None"), proceduralMesh->GetComponentLocation(), true);
	}
	return true;
}

bool AMeshActor::Drop_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("Reacting to Drop"));
	if (!bIsImportedMesh) {
		GetStaticMeshComponent()->SetSimulatePhysics(bSimulatePhysics);
		RootComponent->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
		GetStaticMeshComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
	}
	else {
		proceduralMesh->SetSimulatePhysics(bSimulatePhysics);
		proceduralMesh->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
		proceduralMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
	}
	
	return true;
}

