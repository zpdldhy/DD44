#pragma once
#include "AssimpLoader.h"
class UMeshComponent;
class UMeshResources;

class ObjectLoader
{
	AssimpLoader objectLoader;
	vector<shared_ptr<APawn>> m_vObjList; 
	vector<shared_ptr<UMeshComponent>> m_vMeshList;
	map<wstring, shared_ptr<UMeshResources>> m_mMeshMap;
public:
	vector<shared_ptr<APawn>> Load();
	vector<shared_ptr<UMeshComponent>> LoadMesh();
	map<wstring, shared_ptr<UMeshResources>> LoadMeshMap();
};

