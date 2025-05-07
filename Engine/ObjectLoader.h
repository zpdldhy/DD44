#pragma once
#include "AssimpLoader.h"
class UMeshComponent;
class ObjectLoader
{
	AssimpLoader objectLoader;
	vector<shared_ptr<APawn>> m_vObjList; 
	vector<shared_ptr<UMeshComponent>> m_vMeshList;
public:
	vector<shared_ptr<APawn>> Load();
	vector<shared_ptr<UMeshComponent>> LoadMesh();
};

