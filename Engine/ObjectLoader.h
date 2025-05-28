#pragma once
#include "AssimpLoader.h"
#include "FbxLoader.h"
class UMeshComponent;
class UMeshResources;
class APawn;

// ActorLoader�� ���Ե�
// �� ��� ���� Ŭ����
class ObjectLoader
{
	AssimpLoader objectLoader;
	vector<shared_ptr<APawn>> m_vObjList; 
	vector<shared_ptr<UMeshComponent>> m_vMeshList;
	map<wstring, shared_ptr<UMeshResources>> m_mMeshMap;
public:
	// ActorLoader�� ���Ե�
	// �� ��� ���� Ŭ����
	vector<shared_ptr<APawn>> Load();
	void ConvertObjToAsset(string _path = "../Resources/Obj/*.obj");
	vector<shared_ptr<UMeshComponent>> LoadMesh();
	map<wstring, shared_ptr<UMeshResources>> LoadMeshMap();
	void LoadAsFR(string path);
};

