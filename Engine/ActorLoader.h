#pragma once
#include "APawn.h"
#include "FbxLoader.h"
#include "SkeletalMeshData.h"
#include "UMeshComponent.h"
#include "UAnimInstance.h"
#include "UMaterial.h"

class ActorLoader
{
public:
	FbxLoader m_FbxImporter;
	vector<TFbxResource> m_vFbxList;
	vector<shared_ptr<UMeshComponent>> m_vMeshList;
	vector<shared_ptr<UAnimInstance>> m_vAnimInstanceList;
public:
	void ConvertFbxToAsset(string _path = "../Resources/Fbx/DeathDoor/*.fbx");
	void LoadAllAsset();
	void LoadOne(string _path);
	vector<shared_ptr<UMeshComponent>> LoadMesh();
	vector<wstring> LoadTexPath();
	vector<shared_ptr<UAnimInstance>> LoadAnim();

	// prefab전 바로 사용을 위한 
	shared_ptr<APawn> LoadOneActor(string _path);
	vector<shared_ptr<APawn>> LoadAllActor();

};

