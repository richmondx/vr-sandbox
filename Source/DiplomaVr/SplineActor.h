// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "Classes/Components/SplineComponent.h"
#include "Classes/Components/DecalComponent.h"
#include "Classes/Components/SplineMeshComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Classes/Kismet/GameplayStatics.h"

#include "SplineActor.generated.h"

UCLASS()
class DIPLOMAVR_API ASplineActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASplineActor();

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
	USceneComponent* SceneComponent;

	UPROPERTY(EditAnywhere)
	USplineComponent* Spline;

	UFUNCTION(BlueprintCallable)
	void AddSplinePoint_Decal(UMaterialInterface* DecalMat, float scale, FHitResult Hit);

	UFUNCTION(BlueprintCallable)
	void AddSplinePoint_Mesh(FVector location, UMaterialInterface* meshMat, UStaticMesh* mesh, float fScale);

	UPROPERTY(EditAnywhere, meta = (DisplayName = "Array of placed Static Meshes"))
	TArray<USplineMeshComponent*> splineMeshArray;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	
	
};
