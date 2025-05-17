#pragma once
#include "FbxLoader.h"

class APawn;
class UMeshComponent;
class UMaterial;
class MeshComponentData;

class AAsset
{
public:
	static void Export(TFbxResource _result, string fileName);
	static TFbxResource Load(const char* fileName);
	
	static void ExportMesh(shared_ptr<APawn> _actor, string filepath);
	static void ExportSkinned(FILE* pFile, shared_ptr<UMeshComponent> _mesh);
	static void ExportStatic(FILE* pFile, shared_ptr<UMeshComponent> _mesh);
	
	static MeshComponentData LoadMesh(const char* filepath);
	static MeshComponentData LoadOneMesh(FILE* pFile);

	static void ExportWstring(FILE* pFile, wstring _str);
	static wstring LoadWstring(FILE* pFile);
};

