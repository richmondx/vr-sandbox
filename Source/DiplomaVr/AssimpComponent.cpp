// Fill out your copyright notice in the Description page of Project Settings.

#include "AssimpComponent.h"


// Sets default values for this component's properties
UAssimpComponent::UAssimpComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

}


// Called when the game starts
void UAssimpComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

TArray<FString> UAssimpComponent::openFolder(FString directory)
{
	// save root for future fererence
	rootDirectory = directory;

	TArray<FString> fileList;
	fileList.Empty();
	TArray<FString> supportedFileList;
	supportedFileList.Empty();

	TArray<FString> supportedFiles = {
		"3d", "3ds", "3mf", "ac", "ac3d", "acc", "amj", "ase", "ask", "b3d", "blend",
		"bvh", "cms", "cob", "dae", "collada", "dxf", "enff", "fbx", "gltf", "glb",
		"hmb", "ifc", "step", "ifc-step", "irr",  "irrmesh", "lwo", "lws", "lxo", "md2",
		"md3", "md5", "mdc", "mdl", "mesh",  "mesh.xml", "mot", "ms3d", "ndo", "nff",
		"obj", "off", "ogex", "ply", "pmx", "prj", "q3o", "q3s", "raw", "scn", "sib",
		"smd", "stp", "stl", "ter", "uc", "vta", "x", "x3d", "xgl", "zgl"
	};
	// supportedFiles.Append(extArray, ARRAY_COUNT(extArray));

	if (FPaths::DirectoryExists(directory))
	{
		FString path = directory + "\\*";
		UE_LOG(LogTemp, Warning, TEXT("Resolve folder:: %s\n"), *path);
		FFileManagerGeneric::Get().FindFiles(fileList, *path, true, false);
		for (int i = 0; i < fileList.Num(); i++)
		{
			FString file = fileList[i];
			FString fileName;
			FString fileExtention;
			file.Split(TEXT("."), &fileName, &fileExtention, ESearchCase::IgnoreCase, ESearchDir::FromEnd);
			UE_LOG(LogTemp, Warning, TEXT("Found File: %s [%s]\n"), *file, *fileExtention);

			if (supportedFiles.Contains(*fileExtention.ToLower())) {
				UE_LOG(LogTemp, Warning, TEXT("Supported file: %s - %s"), *fileExtention, *fileList[i]);
				supportedFileList.Add(directory + "\\" + file);
			}
		}
	}
	return supportedFileList;
}																		

bool UAssimpComponent::openMesh(FString path, UMaterial* baseMaterial, int32& SectionCount, FString& ErrorCode, TArray<UMaterialInstanceDynamic*>& importedMaterials)
{
	Assimp::Importer importer;
	std::string filename(TCHAR_TO_UTF8(*path));
	const aiScene* scene = importer.ReadFile(filename, aiProcessPreset_TargetRealtime_MaxQuality);
	if (!scene)
	{
		ErrorCode = importer.GetErrorString();
		return false;
	}
	_meshCurrentlyProcessed = 0;
	materialIds.Empty();
	processNode(scene->mRootNode, scene);
	SectionCount = _meshCurrentlyProcessed;


	if (scene->HasMaterials())
	{
		importedMaterials = ImportMaterials(scene, baseMaterial);
	}
	return true;
}

bool UAssimpComponent::getSection(int32 index, TArray<FVector>& Vertices, TArray<int32>& Faces, TArray<FVector>& Normals, TArray<FVector2D>& UV, TArray<FVector>& Tangents, TArray<FColor>& Colors)
{
	if (index >= _meshCurrentlyProcessed)
	{
		return false;
	}
	Vertices = _vertices[index];
	Faces = _indices[index];
	Normals = _normals[index];
	UV = _uvs[index];
	Tangents = _tangents[index];
	Colors = _vertexColors[index];
	return true;
}

void UAssimpComponent::clear()
{
	_vertices.Empty();
	_indices.Empty();
	_normals.Empty();
	_uvs.Empty();
	_tangents.Empty();
	_vertexColors.Empty();
	_meshCurrentlyProcessed = 0;
}

void UAssimpComponent::processNode(aiNode* node, const aiScene* scene)
{
	for (uint32 i = 0; i < node->mNumMeshes; i++) {
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];

		materialIds.Add(mesh->mMaterialIndex);
		UE_LOG(LogTemp, Warning, TEXT("Mat index: %d\n"), mesh->mMaterialIndex);

		processMesh(mesh, scene);
		++_meshCurrentlyProcessed;
	}
	uint32 nodes = node->mNumMeshes;
	// do the same for all of its children
	for (uint32 i = 0; i < node->mNumChildren; i++) {
		processNode(node->mChildren[i], scene);
	}
}

void UAssimpComponent::processMesh(aiMesh* mesh, const aiScene* scene)
{

	// create the empty arrays
	if (_vertices.Num() <= _meshCurrentlyProcessed) {
		_vertices.AddZeroed();
		_normals.AddZeroed();
		_uvs.AddZeroed();
		_tangents.AddZeroed();
		_vertexColors.AddZeroed();
		_indices.AddZeroed();
	}

	// we check whether the current data to read has a different amount of vertices compared to the last time we generated the mesh
	// if so, it means we'll need to recreate the mesh and resupply new indices.
	if (mesh->mNumVertices != _vertices[_meshCurrentlyProcessed].Num())
		_requiresFullRecreation = true;

	// we reinitialize the arrays for the new data we're reading
	_vertices[_meshCurrentlyProcessed].Empty();
	_normals[_meshCurrentlyProcessed].Empty();
	_uvs[_meshCurrentlyProcessed].Empty();
	_tangents[_meshCurrentlyProcessed].Empty();
	_vertexColors[_meshCurrentlyProcessed].Empty();
	_indices[_meshCurrentlyProcessed].Empty();

	for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
		// Process VERTEX
		FVector vertex;
		// process vertex positions, normals and UVs
		vertex.X = mesh->mVertices[i].x;
		vertex.Y = mesh->mVertices[i].y;
		vertex.Z = mesh->mVertices[i].z;
		_vertices[_meshCurrentlyProcessed].Add(vertex);

		// Process NORMALS
		if (mesh->HasNormals())
		{
			FVector normal = FVector(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
			_normals[_meshCurrentlyProcessed].Add(normal);
		}
		else
		{
			_normals[_meshCurrentlyProcessed].Add(FVector::ZeroVector);
		}


		// if the mesh contains tex coords
		if (mesh->mTextureCoords[0]) {
			FVector2D uvs;
			uvs.X = mesh->mTextureCoords[0][i].x;
			uvs.Y = ( -mesh->mTextureCoords[0][i].y) + 1;
			_uvs[_meshCurrentlyProcessed].Add(uvs);
		}
		else {
			_uvs[_meshCurrentlyProcessed].Add(FVector2D(0.f, 0.f));
		}

	
		if (mesh->HasVertexColors(0))
		{
			FColor color = FColor(
				mesh->mColors[0][i].r,
				mesh->mColors[0][i].g,
				mesh->mColors[0][i].b,
				mesh->mColors[0][i].a
			);
			_vertexColors[_meshCurrentlyProcessed].Add(color);
		}
		
	}

	if (_requiresFullRecreation) {
		// process indices
		for (uint32 i = 0; i < mesh->mNumFaces; i++) {
			aiFace face = mesh->mFaces[i];
			_indices[_meshCurrentlyProcessed].Add(face.mIndices[2]);
			_indices[_meshCurrentlyProcessed].Add(face.mIndices[1]);
			_indices[_meshCurrentlyProcessed].Add(face.mIndices[0]);
		}
	}
}


TArray<UMaterialInstanceDynamic*> UAssimpComponent::ImportMaterials(const aiScene* scene, UMaterial* baseMaterial)
{
	TArray<UMaterialInstanceDynamic*> importedMaterials;
	importedMaterials.Empty();
	meshMaterials.Empty();

	for (uint32 i = 0; i < scene->mNumMaterials; i++)
	{
		aiMaterial* material = scene->mMaterials[i];

		aiString matName;
		if (AI_SUCCESS == material->Get(AI_MATKEY_NAME, matName)) {
			UE_LOG(LogTemp, Warning, TEXT("Processing material: %s\n"), *FString(UTF8_TO_TCHAR(matName.C_Str())));
		}

		UMaterialInstanceDynamic* meshMat = UMaterialInstanceDynamic::Create(baseMaterial, this);

		aiColor3D _colorDiffuse;
		aiColor3D _colorEmissive;
		aiColor3D _colorSpecular;

		if (AI_SUCCESS == material->Get(AI_MATKEY_COLOR_DIFFUSE, _colorDiffuse)) {
			UE_LOG(LogTemp, Warning, TEXT("Diffuse Color: %f %f %f\n"), _colorDiffuse.r, _colorDiffuse.g, _colorDiffuse.b);
			FVector diffuseColor = FVector(_colorDiffuse.r, _colorDiffuse.g, _colorDiffuse.b);
			meshMat->SetVectorParameterValue("BaseColor", diffuseColor);
		}
		if (AI_SUCCESS == material->Get(AI_MATKEY_COLOR_EMISSIVE, _colorEmissive)) {
			UE_LOG(LogTemp, Warning, TEXT("Emissive Color: %f %f %f\n"), _colorEmissive.r, _colorEmissive.g, _colorEmissive.b);
			FVector colorEmissive = FVector(_colorEmissive.r, _colorEmissive.g, _colorEmissive.b);
			meshMat->SetVectorParameterValue("EmissiveColor", colorEmissive);
		}
		if (AI_SUCCESS == material->Get(AI_MATKEY_COLOR_DIFFUSE, _colorSpecular)) {
			UE_LOG(LogTemp, Warning, TEXT("Specular Color: %f %f %f\n"), _colorEmissive.r, _colorEmissive.g, _colorEmissive.b);
			FVector colorSpecular = FVector(_colorSpecular.r, _colorSpecular.g, _colorSpecular.b);
		}

		uint32 _shadingModel;
		if (AI_SUCCESS == material->Get(AI_MATKEY_SHADING_MODEL, _shadingModel)) {
			UE_LOG(LogTemp, Warning, TEXT("Shading Model: %d\n"), _shadingModel);
		}

		uint32 _isTwoSided;
		if (AI_SUCCESS == material->Get(AI_MATKEY_TWOSIDED, _isTwoSided)) {
			UE_LOG(LogTemp, Warning, TEXT("Is two sided: %d\n"), _isTwoSided);
			meshMat->TwoSided = _isTwoSided > 0;
		}

		float _shininess;
		if (AI_SUCCESS == material->Get(AI_MATKEY_SHININESS, _shininess)) {
			UE_LOG(LogTemp, Warning, TEXT("Shininess: %f\n"), _shininess);
			meshMat->SetScalarParameterValue("Roughness", 1 - (_shininess / 1000));
		}

		float _metallic;
		if (AI_SUCCESS == material->Get(AI_MATKEY_GLTF_PBRMETALLICROUGHNESS_METALLIC_FACTOR, _metallic)) {
			UE_LOG(LogTemp, Warning, TEXT("Metallic: %f\n"), _metallic);
		}

		aiString texPath;
		FString texFilepath;

		// Diffuse Texture
			if (material->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
				if (AI_SUCCESS == material->GetTexture(aiTextureType_DIFFUSE, 0, &texPath, nullptr, nullptr, nullptr, nullptr, nullptr)) {
					texFilepath = rootDirectory + "\\" + FString(UTF8_TO_TCHAR(texPath.C_Str()));
					UE_LOG(LogTemp, Warning, TEXT("Diffuse texture found: %s\n"), *texFilepath);
					if (!texFilepath.IsEmpty()) {
						UTexture* diffuseTex = Cast<UTexture>(UKismetRenderingLibrary::ImportFileAsTexture2D(Cast<UObject>(UAssimpComponent::GetWorld()), texFilepath));
						meshMat->SetTextureParameterValue("DiffuseTexture", diffuseTex);
					}
			}
		}

		// Emissive Texture
		if (material->GetTextureCount(aiTextureType_EMISSIVE) > 0) {
			if (AI_SUCCESS == material->GetTexture(aiTextureType_EMISSIVE, 0, &texPath, nullptr, nullptr, nullptr, nullptr, nullptr)) {
				texFilepath = rootDirectory + "\\" + FString(UTF8_TO_TCHAR(texPath.C_Str()));
				UE_LOG(LogTemp, Warning, TEXT("Emissive texture found: %s\n"), *texFilepath);
				if (!texFilepath.IsEmpty()) {
					UTexture* emissiveTex = Cast<UTexture>(UKismetRenderingLibrary::ImportFileAsTexture2D(Cast<UObject>(UAssimpComponent::GetWorld()), texFilepath));
					meshMat->SetTextureParameterValue("EmissiveTexture", emissiveTex);
				}
			}
		}

		// Normal Texture
		if (material->GetTextureCount(aiTextureType_NORMALS) > 0) {
			if (AI_SUCCESS == material->GetTexture(aiTextureType_NORMALS, 0, &texPath, nullptr, nullptr, nullptr, nullptr, nullptr)) {
				texFilepath = rootDirectory + "\\" + FString(UTF8_TO_TCHAR(texPath.C_Str()));
				UE_LOG(LogTemp, Warning, TEXT("Normal texture found: %s\n"), *texFilepath);
				if (!texFilepath.IsEmpty()) {
					UTexture* normalTex = Cast<UTexture>(UKismetRenderingLibrary::ImportFileAsTexture2D(Cast<UObject>(UAssimpComponent::GetWorld()), texFilepath));
					meshMat->SetTextureParameterValue("NormalTexture", normalTex);
				}
			}
		}

		// Specular Texture
		if (material->GetTextureCount(aiTextureType_SPECULAR) > 0) {
			if (AI_SUCCESS == material->GetTexture(aiTextureType_SPECULAR, 0, &texPath, nullptr, nullptr, nullptr, nullptr, nullptr)) {
				texFilepath = rootDirectory + "\\" + FString(UTF8_TO_TCHAR(texPath.C_Str()));
				UE_LOG(LogTemp, Warning, TEXT("SPECULAR texture found: %s\n"), *texFilepath);
			}
		}

		// AO Texture
		if (material->GetTextureCount(aiTextureType_LIGHTMAP) > 0) {
			if (AI_SUCCESS == material->GetTexture(aiTextureType_LIGHTMAP, 0, &texPath, nullptr, nullptr, nullptr, nullptr, nullptr)) {
				texFilepath = rootDirectory + "\\" + FString(UTF8_TO_TCHAR(texPath.C_Str()));
				UE_LOG(LogTemp, Warning, TEXT("LIGHTMAP texture found: %s\n"), *texFilepath);
				if (!texFilepath.IsEmpty()) {
					UTexture* aoTex = Cast<UTexture>(UKismetRenderingLibrary::ImportFileAsTexture2D(Cast<UObject>(UAssimpComponent::GetWorld()), texFilepath));
					meshMat->SetTextureParameterValue("AoTexture", aoTex);
				}
			}
		}

		// Roughness Texture
		if (material->GetTextureCount(aiTextureType_SHININESS) > 0) {
			if (AI_SUCCESS == material->GetTexture(aiTextureType_SHININESS, 0, &texPath, nullptr, nullptr, nullptr, nullptr, nullptr)) {
				texFilepath = rootDirectory + "\\" + FString(UTF8_TO_TCHAR(texPath.C_Str()));
				UE_LOG(LogTemp, Warning, TEXT("SHININESS texture found: %s\n"), *texFilepath);
			}
		}

		// Unknown Texture Type
		if (material->GetTextureCount(aiTextureType_UNKNOWN) > 0) {
			if (AI_SUCCESS == material->GetTexture(aiTextureType_UNKNOWN, 0, &texPath, nullptr, nullptr, nullptr, nullptr, nullptr)) {
				FString filename = FString(UTF8_TO_TCHAR(texPath.C_Str()));
				texFilepath = rootDirectory + "\\" + filename;
				UE_LOG(LogTemp, Warning, TEXT("UNKNOWN texture found: %s\n"), *texFilepath);

				if (filename.Contains("metal", ESearchCase::IgnoreCase))
				{
					UE_LOG(LogTemp, Warning, TEXT("Try texture as metallic map: %s\n"), *texFilepath);
					if (!texFilepath.IsEmpty()) {
						UTexture* metallicTex = Cast<UTexture>(UKismetRenderingLibrary::ImportFileAsTexture2D(Cast<UObject>(UAssimpComponent::GetWorld()), texFilepath));
						meshMat->SetTextureParameterValue("MetallicTexture", metallicTex);
					}

				}
				if (filename.Contains("rough", ESearchCase::IgnoreCase))
				{
					UE_LOG(LogTemp, Warning, TEXT("Try texture as roughness map: %s\n"), *texFilepath);
					if (!texFilepath.IsEmpty()) {
						UTexture* roughnessTex = Cast<UTexture>(UKismetRenderingLibrary::ImportFileAsTexture2D(Cast<UObject>(UAssimpComponent::GetWorld()), texFilepath));
						meshMat->SetTextureParameterValue("RoughnessTexture", roughnessTex);
					}
				}
			}
		}


		importedMaterials.Add(meshMat);
		UE_LOG(LogTemp, Warning, TEXT("Add material: %d\n\n"), importedMaterials.Num());

	}

	return importedMaterials;
}



