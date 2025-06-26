#pragma once
#include "FbxLoader.h"

class APawn;
class AActor;
class UMeshComponent;
class UMaterial;
struct MeshComponentData;

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


	static void ExportJsonMesh(shared_ptr<AActor> _actor, string filepath);
	static void ExportJsonMeshRecursive(string idx, nlohmann::ordered_json& j, shared_ptr<UMeshComponent> _mesh);
	static void ExportSkinned(string idx ,nlohmann::ordered_json& j, shared_ptr<UMeshComponent> _mesh);
	static void ExportStatic(string idx, nlohmann::ordered_json& j, shared_ptr<UMeshComponent> _mesh);

	static MeshComponentData LoadJsonMesh(const char* filepath);
	static MeshComponentData LoadOneJsonMeshRecursive(string idx, const nlohmann::ordered_json& j);
	static MeshComponentData LoadOneJsonMesh(string idx, const nlohmann::ordered_json& j);

	// Helper Methods
	static void ExportWstring(FILE* pFile, wstring _str);
	static wstring LoadWstring(FILE* pFile);
	static json  MatrixToJson(const Matrix& mat);
	static Matrix LoadMatrixFromJson(const nlohmann::ordered_json& j);

};

