// Fill out your copyright notice in the Description page of Project Settings.

#include "ActionController.h"

// Sets default values for this component's properties
UActionController::UActionController()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	bIsFiring = false;
	grabbedActor = nullptr;
	grabHandle = nullptr;

	SpawnMeshIndex = 0;
	SpawnMeshScale = 1.0f;
	bSimulatePhysics = true;

	SphereMaterials = { nullptr, nullptr, nullptr, nullptr };

}


// Called when the game starts
void UActionController::BeginPlay()
{
	Super::BeginPlay();
	
	UE_LOG(LogTemp, Warning, TEXT("Parent: %s\n"), *GetOwner()->GetName());
	if (GetOwner())
	{
		grabHandle = GetOwner()->FindComponentByClass<UPhysicsHandleComponent>();
		if (grabHandle)
		{
			UE_LOG(LogTemp, Warning, TEXT("Handle Found!\n"));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Handle NOT Found!\n"));
		}
	}

	ChangeMode(0, false);	// set mode & line distance
}


// Called every frame
void UActionController::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bIsFiring)
	{
		SpinMaterialSphere(2.6);
		UE_LOG(LogTemp, Warning, TEXT("MODE: %d\n"), mode);
		switch (mode)
		{
		case 0:
			WallPaint();
			break;

		case 1:
			SpacePaint();
			break;

		case 2:
			SpawnMesh();
			bIsFiring = false;
			break;
			
		case 3:
			GrabActor();
			break;

		default:
			break;
		}
	}
	else
	{
		SpinMaterialSphere(.8);
	}
}

bool UActionController::InitReferences(UStaticMeshComponent * MateralSphereRef, UArrowComponent * ArrowComponentRef)
{
	if (MateralSphereRef == nullptr || ArrowComponentRef == nullptr) {
		return false;
	}

	// ControllerMesh = ControllerMeshRef;
	MaterialSphere = MateralSphereRef;
	ArrowComponent = ArrowComponentRef;


	return true;
}

bool UActionController::InitMaterials(UMaterial* BaseDecalMat, TArray<UMaterial*>SplineBaseMaterials)
{
	// Init materials
	if (BaseDecalMat == nullptr)
	{
		return false;
	}
	// Init Wall paint materials
	// Iterate through colors and decal textures
	UMaterial* BaseDecalMaterial = nullptr;
	UMaterialInstanceDynamic* generatedMaterial;
	for (FLinearColor color : MaterialColors)
	{
		for (UTexture2D* decalMask : DecalMaskTextures)
		{
			// UE_LOG(LogTemp, Warning, TEXT("Create new dynamic instance"));

			// Create new dynamic instances and set parameters (color and decal textures)
			generatedMaterial = UMaterialInstanceDynamic::Create(BaseDecalMat, this);
			generatedMaterial->SetVectorParameterValue("DecalColor", color);
			generatedMaterial->SetTextureParameterValue("DecalMaskTexture", decalMask);
			WallPaintBrush.materialList.Add(generatedMaterial);
		}
	}
	WallPaintBrush.activeMaterial = WallPaintBrush.materialList.Num() > 0 ? WallPaintBrush.materialList[0] : nullptr;
	WallPaintBrush.brushColorIndex = 0;
	WallPaintBrush.brushTypeIndex = 0;
	WallPaintBrush.brushSize = 10.0f;
	

	// Init Space paint materials
	// Iterate through colors and meshes textures
	for (FLinearColor color : MaterialColors)
	{
		for (UMaterial* baseMaterial : SplineBaseMaterials)
		{
			// Create new dynamic instances and set parameters (color and decal textures)
			generatedMaterial = UMaterialInstanceDynamic::Create(baseMaterial, this);
			generatedMaterial->SetVectorParameterValue("BaseColor", color);
			SpacePaintBrush.materialList.Add(generatedMaterial);
		}
	}
	SpacePaintBrush.activeMaterial = SpacePaintBrush.materialList.Num() > 0 ? SpacePaintBrush.materialList[0] : nullptr;
	SpacePaintBrush.brushColorIndex = 0;
	SpacePaintBrush.brushTypeIndex = 0;
	SpacePaintBrush.brushSize = 0.5f;

	if (SplineMeshes.Num() > 0 && SplineMeshes[0] != nullptr)
	{
		SpacePaintMesh = SplineMeshes[0];

	}

	UE_LOG(LogTemp, Warning, TEXT("DONE"));

	return true;
}

void UActionController::SpinMaterialSphere(float speed)
{
	if (MaterialSphere)
	{
		FRotator currentRotation = (FRotator)MaterialSphere->GetRelativeTransform().GetRotation();
		FRotator addRotation = FRotator(1.0f, 0.3f, 0.7f);
		addRotation = addRotation * speed;

		MaterialSphere->SetRelativeRotation(UKismetMathLibrary::ComposeRotators(currentRotation, addRotation));
	}
}
	

void UActionController::WallPaint()
{
	float traceDistance = lineTraceDistance;
	// Line trace
	FTransform arrowTransform = ArrowComponent->GetComponentTransform();
	FVector startLoc = arrowTransform.GetLocation();
	FVector endLoc = startLoc + ArrowComponent->GetForwardVector() * traceDistance;


	

	FCollisionQueryParams TraceParams = FCollisionQueryParams(FName(TEXT("LineTrace")), true);
	TraceParams.bTraceComplex = true;
	TraceParams.bTraceAsyncScene = true;
	TraceParams.bReturnPhysicalMaterial = false;
	//const FName TraceTag("MyTraceTag");
	//GetWorld()->DebugDrawTraceTag = TraceTag;
	//TraceParams.TraceTag = TraceTag;
	TraceParams.AddIgnoredActor(GetOwner());
	

	//Re-initialize hit info
	FHitResult Hit;

	//call GetWorld() from within an actor extending class
	bool isHit = GetWorld()->LineTraceSingleByChannel(
		Hit,
		startLoc,
		endLoc,
		ECollisionChannel::ECC_Camera,
		TraceParams
	);

	if (isHit)
	{
		if (Hit.bBlockingHit)
		{
			if (splineActorRef == nullptr) {
				splineActorRef = CreateSplineActor();
			}

			splineActorRef->AddSplinePoint_Decal(WallPaintBrush.activeMaterial, WallPaintBrush.brushSize, Hit);
		}
	}
	else {
		splineActorRef = nullptr;
	}



}

void UActionController::SpacePaint()
{
	float traceDistance = lineTraceDistance;
	// Line trace
	FTransform arrowTransform = ArrowComponent->GetComponentTransform();
	FVector startLoc = arrowTransform.GetLocation();
	FVector endLoc = startLoc + ArrowComponent->GetForwardVector() * traceDistance;

	if (splineActorRef == nullptr) {
		splineActorRef = CreateSplineActor();
	}
	if (SpacePaintMesh != nullptr)
	{
		splineActorRef->AddSplinePoint_Mesh(endLoc, SpacePaintBrush.activeMaterial, SpacePaintMesh, SpacePaintBrush.brushSize);
	}
	
}

ASplineActor* UActionController::CreateSplineActor()
{
	float distance = 200.0f;

	FVector spawnLocation = ArrowComponent->GetComponentTransform().GetLocation() + (ArrowComponent->GetForwardVector() * distance);
	FRotator spawnRotation = (FRotator)ArrowComponent->GetComponentTransform().GetRotation();
	ASplineActor* spawnedSpline = GetWorld()->SpawnActor<ASplineActor>(spawnLocation, spawnRotation);

	return spawnedSpline;
}

void UActionController::SpawnMesh()
{
	float spawnDistance = lineTraceDistance * abs(SpawnMeshScale);
	// Line trace
	FTransform arrowTransform = ArrowComponent->GetComponentTransform();
	FVector spawnLoc;

	FVector startLoc = arrowTransform.GetLocation();
	FVector endLoc = startLoc + ArrowComponent->GetForwardVector() * spawnDistance;

	FCollisionQueryParams TraceParams = FCollisionQueryParams(FName(TEXT("LineTrace")), true);
	TraceParams.bTraceComplex = true;
	TraceParams.bTraceAsyncScene = true;
	TraceParams.bReturnPhysicalMaterial = false;
	const FName TraceTag("MyTraceTag");
	// GetWorld()->DebugDrawTraceTag = TraceTag;
	TraceParams.TraceTag = TraceTag;
	TraceParams.AddIgnoredActor(GetOwner());


	//Re-initialize hit info
	FHitResult Hit;

	//call GetWorld() from within an actor extending class
	bool isHit = GetWorld()->LineTraceSingleByChannel(
		Hit,
		startLoc,
		endLoc,
		ECollisionChannel::ECC_Camera,
		TraceParams
	);

	if (isHit && Hit.bBlockingHit) {
		UE_LOG(LogTemp, Warning, TEXT("Hit"));

		spawnLoc = Hit.Location;	
	}
	else {
		spawnLoc = endLoc;
	}

	if (SpawnMeshIndex < SpawnableMeshesBasic.Num() + SpawnableMeshesImported.Num())
	{
		FRotator spawnRotation = (FRotator)ArrowComponent->GetComponentTransform().GetRotation();
		AMeshActor* spawnedMesh = GetWorld()->SpawnActor<AMeshActor>(spawnLoc, spawnRotation);
		
		FVector spawnScale = FVector(SpawnMeshScale, SpawnMeshScale, SpawnMeshScale);
		spawnedMesh->SetActorRelativeScale3D(spawnScale);

		if (SpawnMeshIndex < SpawnableMeshesBasic.Num())
		{
			spawnedMesh->GetStaticMeshComponent()->SetSimulatePhysics(true);
			spawnedMesh->GetStaticMeshComponent()->SetStaticMesh(SpawnableMeshesBasic[SpawnMeshIndex]);
		}
		else
		{
			spawnedMesh->bIsImportedMesh = true;
			int32 meshIndex = SpawnMeshIndex - SpawnableMeshesBasic.Num();
			spawnedMesh->proceduralMesh->SetRuntimeMesh(SpawnableMeshesImported[meshIndex]->GetRuntimeMesh());
			for (uint8 i = 0; i < SpawnableMeshesImported[meshIndex]->GetNumMaterials(); i++)
			{
				spawnedMesh->proceduralMesh->SetMaterial(i, SpawnableMeshesImported[meshIndex]->GetMaterial(i));
			}
			spawnedMesh->proceduralMesh->SetSimulatePhysics(true);
			spawnedMesh->proceduralMesh->SetCollisionUseComplexAsSimple(false);
			// spawnedMesh->proceduralMesh->GetRuntimeMesh()->SetCollisionUseComplexAsSimple(false);
		}
		
		spawnedMesh->bIsSpawned = true;

	}
	
}

void UActionController::ChangeActionProperty(EActionPropertyEnum property, int32 valueChange)
{
	float newValue = 0;

	switch (mode)
	{
		case 0:
		{
			if (property == EActionPropertyEnum::AP_Color)
			{
				newValue = WallPaintBrush.brushColorIndex + valueChange;
				if (newValue >= 0 && newValue < MaterialColors.Num())
				{
					WallPaintBrush.brushColorIndex = newValue;
				}
			}
			else if (property == EActionPropertyEnum::AP_Type)
			{
				newValue = WallPaintBrush.brushTypeIndex + valueChange;
				if (newValue >= 0 && newValue < DecalMaskTextures.Num())
				{
					WallPaintBrush.brushTypeIndex = newValue;
				}
			}
			else if (property == EActionPropertyEnum::AP_Size)
			{
				newValue = WallPaintBrush.brushSize + valueChange;
				if (newValue > 8 && newValue <= 80.0f)
				{
					WallPaintBrush.brushSize = newValue;
				}
			}
			UE_LOG(LogTemp, Warning, TEXT("Color Index: %f, Type index: %f"), WallPaintBrush.brushColorIndex, WallPaintBrush.brushTypeIndex);
			int32 matIndex = WallPaintBrush.brushColorIndex * DecalMaskTextures.Num() + WallPaintBrush.brushTypeIndex;

			if (matIndex < WallPaintBrush.materialList.Num()) {
				WallPaintBrush.activeMaterial = WallPaintBrush.materialList[matIndex];
			}

			break;
		}
		case 1:
		{
			if (property == EActionPropertyEnum::AP_Color)
			{
				newValue = SpacePaintBrush.brushColorIndex + valueChange;
				if (newValue >= 0 && newValue < MaterialColors.Num())
				{
					SpacePaintBrush.brushColorIndex = newValue;
				}
			}
			else if (property == EActionPropertyEnum::AP_Type)
			{
				newValue = SpacePaintBrush.brushTypeIndex + valueChange;
				if (newValue >= 0 && newValue < SpacePaintBrush.materialList.Num() / MaterialColors.Num())
				{
					SpacePaintBrush.brushTypeIndex = newValue;
					// SpacePaintMesh = SplineMeshes[SpacePaintBrush.brushTypeIndex];
				}
			}
			if (property == EActionPropertyEnum::AP_Size)
			{
				newValue = SpacePaintBrush.brushSize + valueChange * .05f;
				if (newValue > 0.05f && newValue <= 1.0f)
				{
					SpacePaintBrush.brushSize = newValue;
				}
			}

			int32 matIndex = SpacePaintBrush.brushColorIndex * (SpacePaintBrush.materialList.Num() / MaterialColors.Num()) + SpacePaintBrush.brushTypeIndex;
			UE_LOG(LogTemp, Warning, TEXT("Color Index: %f, Type index: %f, index: %f"), SpacePaintBrush.brushColorIndex, SpacePaintBrush.brushTypeIndex, matIndex);
			if (matIndex < SpacePaintBrush.materialList.Num()) {
				SpacePaintBrush.activeMaterial = SpacePaintBrush.materialList[matIndex];
			}

			UE_LOG(LogTemp, Warning, TEXT("Space brush valueChange: %f"), SpacePaintBrush.brushSize);

			break;
		}
		case 2:
		{
			if (property == EActionPropertyEnum::AP_Color)
			{
				newValue = SpawnMeshIndex + valueChange;
				if (newValue >= 0 && newValue < SpawnableMeshesBasic.Num() + SpawnableMeshesImported.Num() )
				{
					SpawnMeshIndex = newValue;
				}
				UE_LOG(LogTemp, Warning, TEXT("SPawn mesh index: %d"), SpawnMeshIndex);
			}

			else if (property == EActionPropertyEnum::AP_Size)
			{
				SpawnMeshScale += valueChange * 0.5f;
			}

		}
		case 3:
		{

			if (property == EActionPropertyEnum::AP_Color)
			{
				bSimulatePhysics = valueChange > 0;
				UE_LOG(LogTemp, Warning, TEXT("Simulate physics: %s"), bSimulatePhysics ? TEXT("True") : TEXT("False"));
			}
			break;

		}
		default:
			break;
	}
}

void UActionController::CleanUp()
{
	if (splineActorRef)
	{
		splineActorRef = nullptr;
	}

	if (grabbedActor)
	{
		DropActor();
	}
}

AActor* UActionController::GrabActor()
{	
	// if we are already grabbing a mesh, return it
	if (grabbedActor) {

		if (grabHandle && grabHandle->GrabbedComponent)
		{
			FVector handlePoint = ArrowComponent->GetComponentLocation() + (ArrowComponent->GetForwardVector() * lineTraceDistance);
			grabHandle->SetTargetLocation(handlePoint);
		}
		return grabbedActor;
	}


	float traceDistance = lineTraceDistance;

	// Line trace
	FTransform arrowTransform = ArrowComponent->GetComponentTransform();
	FVector startLoc = arrowTransform.GetLocation();
	FVector endLoc = startLoc + ArrowComponent->GetForwardVector() * traceDistance;

	FCollisionQueryParams TraceParams = FCollisionQueryParams(FName(TEXT("LineTrace")), true);
	TraceParams.bTraceComplex = false;
	TraceParams.bReturnPhysicalMaterial = false;

	// Ignore self
	TraceParams.AddIgnoredActor(GetOwner());

	//Re-initialize hit info
	FHitResult Hit;

	bool isHit = GetWorld()->LineTraceSingleByChannel(
		Hit,
		startLoc,
		endLoc,
		ECollisionChannel::ECC_Visibility,
		TraceParams
	);

	if (isHit)
	{
		if (Hit.bBlockingHit && Hit.Actor != nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("Hit mesh: %s, %s"), *Hit.Actor->GetName(), *Hit.Component->GetName());

			if (Hit.Actor->GetClass()->ImplementsInterface(UInteractionInterface::StaticClass()))
			{
				UE_LOG(LogTemp, Warning, TEXT("Can be grabbed :D"));
				IInteractionInterface* ActorInterface = Cast<IInteractionInterface>(Hit.Actor);
				grabbedActor = Hit.GetActor();
				if (grabHandle)
				{
					UE_LOG(LogTemp, Warning, TEXT("Activete handle..."));
					ActorInterface->Execute_PickUpByHandle(grabbedActor, grabHandle);
				}
				else
				{
					ActorInterface->Execute_PickUp(grabbedActor, ArrowComponent);
				}
			}
			//splineActorRef->AddSplinePoint_Decal(WallPaintBrush.activeMaterial, WallPaintBrush.brushSize, Hit);
		}
	}

	return grabbedActor;
}

void UActionController::DropActor()
{
	IInteractionInterface* ActorInterface = Cast<IInteractionInterface>(grabbedActor);

	AMeshActor* meshActor = Cast<AMeshActor>(grabbedActor);
	if (meshActor)
	{
		meshActor->bSimulatePhysics = bSimulatePhysics;
	}
	ActorInterface->Execute_Drop(grabbedActor);

	if (grabHandle && grabHandle->GrabbedComponent)
	{
		grabHandle->ReleaseComponent();
	}
	grabbedActor = nullptr;
}

int32 UActionController::ChangeMode(int32 modeChange, bool bIsIncrement)
{	
	int32 newMode;
	if (!bIsIncrement)
	{
		newMode = modeChange;
	}

	else
	{
		newMode = mode + modeChange;

		if (bIsIncrement && newMode < 0)
		{
			newMode = 3;
		}
		else if (bIsIncrement && newMode > 3)
		{
			newMode = 0;
		}
	}

	if (newMode >= 0 && newMode <= 3)
	{
		mode = newMode;
	}

	switch (mode)
	{
	case 0:
		lineTraceDistance = 400.0f;
		break;
	case 1:
		lineTraceDistance = 200.0f;
		break;
	case 2:
		lineTraceDistance = 350.0f;
		break;
	case 3:
		lineTraceDistance = 300.0f;
		break;
	default:
		lineTraceDistance = 400.0f;
	}

	if (MaterialSphere && SphereMaterials.Num() > mode && SphereMaterials[mode])
	{
		MaterialSphere->SetMaterial(0, SphereMaterials[mode]);
	}

	return mode;
}

