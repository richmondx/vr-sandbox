// Fill out your copyright notice in the Description page of Project Settings.

#include "SplineActor.h"


// Sets default values
ASplineActor::ASplineActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent123"));
	SceneComponent->Mobility = EComponentMobility::Static;
	RootComponent = SceneComponent;

	Spline = CreateDefaultSubobject<USplineComponent>(TEXT("Spline"));
	SceneComponent->Mobility = EComponentMobility::Static;
	Spline->ClearSplinePoints();
}

void ASplineActor::AddSplinePoint_Decal(UMaterialInterface* DecalMat, float scale, FHitResult Hit)
{

	if (DecalMat == nullptr)
	{
		return;
	}
	UE_LOG(LogTemp, Warning, TEXT("test %s, %f, %s"), *DecalMat->GetFName().ToString(), scale, *Hit.Location.ToString());

	if (Hit.Component != nullptr) {
		UE_LOG(LogTemp, Warning, TEXT("HIT COMPONENT"));

		// Add point to spline
		Spline->AddSplinePoint(Hit.Location, ESplineCoordinateSpace::World);
	
		// Spawn decal at hit location and attached to hit component
		UDecalComponent* pointDecal = UGameplayStatics::SpawnDecalAttached(
			DecalMat,
			FVector(2, scale, scale),
			Hit.GetComponent(), "",
			Hit.Location,
			UKismetMathLibrary::MakeRotFromX(Hit.Normal),
			EAttachLocation::KeepWorldPosition
		);

		// pointDecal->SetFadeScreenSize(0.001f);


		UE_LOG(LogTemp, Warning, TEXT("Number of points %d"), Spline->GetNumberOfSplinePoints());
		int32 numOfPoints = Spline->GetNumberOfSplinePoints();

		if (numOfPoints >= 2) {
			float decalSpacing = scale / 2.3f;
			float distanceToLast = Spline->GetDistanceAlongSplineAtSplinePoint(numOfPoints - 1);
			float distanceToFirst = Spline->GetDistanceAlongSplineAtSplinePoint(numOfPoints - 2);
			float distanceDelta = distanceToLast - distanceToFirst;


			for (int32 i = 1; i <= UKismetMathLibrary::FTrunc(distanceDelta / decalSpacing); i++)
			{
				float fillSplineDistance = distanceToFirst + i * decalSpacing;
				FTransform decalTransform = Spline->GetTransformAtDistanceAlongSpline(fillSplineDistance, ESplineCoordinateSpace::World);

				pointDecal = UGameplayStatics::SpawnDecalAttached(
					DecalMat,
					FVector(2, scale, scale),
					Hit.GetComponent(), "",
					decalTransform.GetLocation(),
					UKismetMathLibrary::MakeRotFromX(Hit.Normal),
					EAttachLocation::KeepWorldPosition
				);
				// pointDecal->SetFadeScreenSize(0.001f);
			}
		}
	}
	

}

void ASplineActor::AddSplinePoint_Mesh(FVector location, UMaterialInterface * meshMat, UStaticMesh * mesh, float scale)
{

	float MIN_POINT_DISTANCE = 3.0f;
	bool hasMeshes = splineMeshArray.Num() > 0;
	int32 numOfPoints = Spline->GetNumberOfSplinePoints();
	
	FVector lastPointLoc = Spline->GetLocationAtSplinePoint(numOfPoints - 1, ESplineCoordinateSpace::Local);
	
	//if (hasMesh) {
	//	UE_LOG(LogTemp, Warning, TEXT("Distance1: %s, Distance2: %s, delta: %f"),
	//		*splineMeshArray.Last()->GetEndPosition().ToString(),
	//		*location.ToString(),
	//		FVector::Distance(splineMeshArray.Last()->GetEndPosition(), location)
	//	);
	//}
	// Check if latest spline mesh location is too close to new location
	if (hasMeshes && FVector::Distance(splineMeshArray.Last()->GetEndPosition(), location) < MIN_POINT_DISTANCE) {
		UE_LOG(LogTemp, Warning, TEXT("Distance: %f"), FVector::Distance(splineMeshArray.Last()->GetRelativeTransform().GetLocation(), location));
		return;
	}

	//UE_LOG(LogTemp, Warning, TEXT("Rotation1: %s, Rotation2: %s"), *Spline->GetRotationAtSplinePoint(numOfPoints - 1, ESplineCoordinateSpace::Local).ToString(), *Spline->GetRotationAtSplinePoint(numOfPoints - 2, ESplineCoordinateSpace::Local).ToString() );

	// Add point to spline
	Spline->AddSplinePoint(location, ESplineCoordinateSpace::World);
	UE_LOG(LogTemp, Warning, TEXT("Number of points %d"), numOfPoints);

	USplineMeshComponent* splineMesh = nullptr;
	if (numOfPoints > 2) {
		splineMesh = NewObject<USplineMeshComponent>(USplineMeshComponent::StaticClass());
		splineMesh->SetMobility(EComponentMobility::Movable);
		splineMesh->SetStaticMesh(mesh);
		FVector2D scaleVector = FVector2D(scale, scale);
		splineMesh->SetStartScale(scaleVector);
		splineMesh->SetEndScale(scaleVector);
		splineMesh->SetMaterial(0, meshMat);

		//splineMesh->CastShadow = false;
		
		FAttachmentTransformRules transRules = FAttachmentTransformRules(EAttachmentRule::KeepRelative, true);
		splineMesh->AttachToComponent(Spline, transRules);

		//splineMesh->CanCharacterStepUpOn = ECanBeCharacterBase::ECB_No;
		splineMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		splineMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Block);
		splineMesh->SetMobility(EComponentMobility::Stationary);
		splineMesh->CastShadow = false;
		splineMesh->bCastStaticShadow = false;
		splineMesh->bCastFarShadow = false;

		FVector startLocation = Spline->GetLocationAtSplinePoint(numOfPoints - 2, ESplineCoordinateSpace::Local);
		FVector startTangent = Spline->GetTangentAtSplinePoint(numOfPoints - 2, ESplineCoordinateSpace::Local);
		FVector endLocation = Spline->GetLocationAtSplinePoint(numOfPoints - 1, ESplineCoordinateSpace::Local);
		FVector endTangent = Spline->GetTangentAtSplinePoint(numOfPoints - 1, ESplineCoordinateSpace::Local);
		UE_LOG(LogTemp, Warning, TEXT("Locations: %s %s, %s %s"), *startLocation.ToString(), *startTangent.ToString(), *endLocation.ToString(), *endTangent.ToString());
		Spline->GetLocationAndTangentAtSplinePoint(numOfPoints - 2, startLocation, startTangent, ESplineCoordinateSpace::Local);
		Spline->GetLocationAndTangentAtSplinePoint(numOfPoints - 1, endLocation, endTangent, ESplineCoordinateSpace::Local);
		splineMesh->SetStartAndEnd(startLocation, startTangent, endLocation, endTangent);
		

		UE_LOG(LogTemp, Warning, TEXT("Number of meshes %d"), splineMeshArray.Num());
		if (hasMeshes)
		{
			splineMeshArray.Last()->SetEndTangent(startTangent);
		}
		
		splineMesh->RegisterComponentWithWorld(GetWorld());

		splineMeshArray.Add(splineMesh);
		
	}
}

// Called when the game starts or when spawned
void ASplineActor::BeginPlay()
{
	Super::BeginPlay();
		
}

// Called every frame
void ASplineActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

