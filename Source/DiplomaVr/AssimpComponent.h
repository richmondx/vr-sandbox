// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/material.h>
#include <assimp/pbrmaterial.h>
#include <Classes/Materials/MaterialInstanceDynamic.h>
#include <Classes/Engine/Texture.h>
#include "Components/ActorComponent.h"
#include <Components/MeshComponent.h>
#include <HAL/FileManagerGeneric.h>
#include <Classes/Kismet/KismetRenderingLibrary.h>
#include "AssimpComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class DIPLOMAVR_API UAssimpComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UAssimpComponent();

	UPROPERTY(VisibleAnywhere, Category = "Assimp")
	TArray<UMeshComponent*> importedMeshes;

	UPROPERTY(VisibleAnywhere, Category = "Assimp")
	TArray<UMaterialInstanceDynamic*> meshMaterials;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Assimp")
	TArray<int32> materialIds;

	UFUNCTION(BlueprintCallable, Category = "Assimp")
	TArray<FString> openFolder(FString directory);

	UFUNCTION(BlueprintCallable, Category = "Assimp")
	bool openMesh(FString path, UMaterial* baseMaterial, int32& SectionCount, FString& ErrorCode, TArray<UMaterialInstanceDynamic*>& importedMaterials);

	UFUNCTION(BlueprintCallable, Category = "Assimp")
	bool getSection(int32 index, TArray<FVector>& Vertices, TArray<int32>& Faces, TArray<FVector>& Normals, TArray<FVector2D>& UV, TArray<FVector>& Tangents, TArray<FColor>& Colors);
		
	/*UFUNCTION(BlueprintCallable, Category = "Assimp")
	UStaticMesh* meshComponentToStaticMesh(URuntimeMeshComponent* RuntimeMeshComp);*/

	UFUNCTION(BlueprintCallable, Category = "Assimp")
	void clear();

	private:
		int32 _selectedVertex;
		int32 _meshCurrentlyProcessed;
		bool _addModifier;
		int _lastModifiedTime;
		bool _requiresFullRecreation;

		TArray<TArray<FVector>> _vertices;
		TArray<TArray<int32>> _indices;
		TArray<TArray<FVector>> _normals;
		TArray<TArray<FVector2D>> _uvs;
		TArray<TArray<FVector>> _tangents;
		TArray<TArray<FColor>> _vertexColors;

		FString rootDirectory;

		void processMesh(aiMesh* mesh, const aiScene* scene);
		void processNode(aiNode* node, const aiScene* scene);
		TArray<UMaterialInstanceDynamic*> ImportMaterials(const aiScene* scene, UMaterial* baseMaterial);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	
	
};
