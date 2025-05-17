#pragma once
#include "APawn.h"
#include "FbxLoader.h"
#include "SkeletalMeshData.h"
#include "UMeshComponent.h"
#include "UAnimInstance.h"
#include "UMaterial.h"
#include "UMeshResources.h"

class ActorLoader
{
public:
	FbxLoader m_FbxImporter;
	vector<TFbxResource> m_vFbxList;
	vector<MeshComponentData> m_vMeshDataList;
	vector<shared_ptr<UMeshComponent>> m_vMeshList;
	vector<shared_ptr<UMeshResources>> m_vMeshResList;
	map<wstring, shared_ptr<UMeshResources>> m_mMeshMap;
	vector<shared_ptr<UAnimInstance>> m_vAnimInstanceList;
	map<wstring, shared_ptr<UAnimInstance>> m_vAnimInstanceMap;
public:
	void ConvertFbxToAsset(string _path = "../Resources/Fbx/DeathDoor/*.fbx");
	void LoadAllAsset();
	void LoadOne(string _path);
	vector<MeshComponentData> LoadMeshData();
	vector<shared_ptr<UMeshComponent>> LoadMesh();
	map<wstring, shared_ptr<UMeshResources>> LoadMeshMap();
	vector<shared_ptr<UMeshResources>> LoadMeshResources();
	vector<wstring> LoadTexPath();
	vector<shared_ptr<UAnimInstance>> LoadAnim();
	map < wstring, shared_ptr<UAnimInstance>> LoadAnimMap();

	// prefab전 바로 사용을 위한 
	shared_ptr<APawn> LoadOneActor(string _path);
	vector<shared_ptr<APawn>> LoadAllActor();

};

