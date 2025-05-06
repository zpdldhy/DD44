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
	vector<ParsingData> m_vFbxList;
	vector<shared_ptr<UMeshComponent>> m_vMeshList;
	//vector<shared_ptr<UAnimInstance>> m_vAnimInstanceList;
	//vector<shared_ptr<UMaterial>> m_vSkinnedMaterialList;
	//vector<shared_ptr<UMaterial>> m_vStaticMaterialList; 
public:
	void ConvertFbxToAsset(string _path = "../Resources/Fbx/DeathDoor/*.fbx");
	vector<shared_ptr<APawn>> Load();
	vector<shared_ptr<UMeshComponent>> LoadMesh();
	//vector<shared_ptr<UAnimInstance>> LoadAnim();
	//vector<shared_ptr<UMaterial>> LoadSkinnedMaterial();
	//vector<shared_ptr<UMaterial>> LoadStaticMaterial();
};

