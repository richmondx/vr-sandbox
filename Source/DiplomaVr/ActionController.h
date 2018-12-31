// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <CoreMinimal.h>
#include <Components/ActorComponent.h>
#include <Classes/Materials/MaterialInstanceDynamic.h>
#include <Classes/Engine/Texture2D.h>
#include <Classes/Components/StaticMeshComponent.h>
#include <Classes/Components/ArrowComponent.h>
#include <SplineActor.h>
#include <MeshActor.h>
#include <InteractionInterface.h>
#include <Classes/Engine/World.h>
#include <Kismet/KismetMathLibrary.h>
#include "RuntimeMeshComponent.h"
#include "ActionController.generated.h"


// Struct for two brushes
USTRUCT(BlueprintType)
struct FBrushOptions {

	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float brushSize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 brushColorIndex;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 brushTypeIndex;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<UMaterialInstanceDynamic*> materialList;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UMaterialInstanceDynamic* activeMaterial;
};

// Enum for property changes
UENUM(BlueprintType)		//"BlueprintType" is essential to include
enum class EActionPropertyEnum : uint8
{
	AP_Color UMETA(DisplayName = "Brush Color"),
	AP_Type UMETA(DisplayName = "Brush Type"),
	AP_Size	UMETA(DisplayName = "Brush Size")
};


UCLASS(Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DIPLOMAVR_API UActionController : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UActionController();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	//UPROPERTY(VisibleAnywhere, meta = (Category = "Components"))
	//UStaticMeshComponent* ControllerMesh;

	UPROPERTY(VisibleAnywhere, meta = (Category = "Components"))
	UStaticMeshComponent* MaterialSphere;

	UPROPERTY(VisibleAnywhere, meta = (Category = "Components"))
	UArrowComponent* ArrowComponent;

	UPROPERTY(BlueprintReadOnly, meta = (ClampMin = "0.0", ClampMax = "5.0", UIMin = "0.0", UIMax = "5.0"), Category = "Base Properties")
	int32 mode;

	UPROPERTY(BlueprintReadWrite, Category = "Base Properties")
	bool bIsFiring;

	// Color variations for decals, materials
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Brush Init")
	TArray<FLinearColor> MaterialColors;

	// Array of decal masks fro different brush variation
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Brush Init")
	TArray<UTexture2D*> DecalMaskTextures;

	// Array of meshes used for spline meshes
	UPROPERTY(EditAnywhere, Category = "Brush Init")
	TArray<UStaticMesh*> SplineMeshes;


	// Array of meshes used for spawning
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Brush Init")
	TArray<UStaticMesh*> SpawnableMeshesBasic;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Brush Init")
	TArray<URuntimeMeshComponent*> SpawnableMeshesImported;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Brush Options")
	int32 SpawnMeshIndex;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Brush Options")
	float SpawnMeshScale;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Brush Options")
	bool bSimulatePhysics;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Brush Options")
	FBrushOptions WallPaintBrush;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Brush Options")
	FBrushOptions SpacePaintBrush;

	UPROPERTY(EditAnywhere, Category = "Brush Options")
	UStaticMesh* SpacePaintMesh;

	
	// Line trace distance for each mode 
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float lineTraceDistance;

	// Class Functions
	UFUNCTION(BlueprintCallable)
	bool InitReferences(UStaticMeshComponent* MateralSphereRef, UArrowComponent* ArrowComponentRef);

	UFUNCTION(BlueprintCallable)
	bool InitMaterials(UMaterial* BaseDecalMat, TArray<UMaterial*>SplineBaseMaterials);

	UFUNCTION(BlueprintCallable)
	void SpinMaterialSphere(float speed);

	UFUNCTION(BlueprintCallable)
	void WallPaint();

	UFUNCTION(BlueprintCallable)
	void SpacePaint();

	UFUNCTION(BlueprintCallable)
	ASplineActor* CreateSplineActor();

	UFUNCTION(BlueprintCallable)
	void SpawnMesh();

	UFUNCTION(BlueprintCallable)
	void ChangeActionProperty(EActionPropertyEnum property, int32 valueChange);

	UFUNCTION(BlueprintCallable)
	void CleanUp();

	UFUNCTION(BlueprintCallable)
	AActor* GrabActor();

	UFUNCTION(BlueprintCallable)
	void DropActor();

	UFUNCTION(BlueprintCallable)
	int32 ChangeMode(int32 modeChange, bool bIsIncrement);


private: 
	AActor* grabbedActor;

	ASplineActor* splineActorRef;

	AMeshActor* spawnedMeshRef;
};
