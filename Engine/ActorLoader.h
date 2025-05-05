#pragma once
#include "APawn.h"
#include "FbxLoader.h"
#include "SkeletalMeshData.h"
#include "UMeshComponent.h"

class ActorLoader
{
public:
	FbxLoader m_FbxImporter;
	vector<ParsingData> fbxList;
	vector<shared_ptr<UMeshComponent>> meshList;
public:
	void ConvertFbxToAsset(string _path = "../Resources/Fbx/DeathDoor/*.fbx");
	vector<shared_ptr<APawn>> Load();
	vector<shared_ptr<UMeshComponent>> LoadMesh();

};

