#include "pch.h"
#include "ObjectLoader.h"
#include "UMaterial.h"
#include "UStaticMeshComponent.h"
#include "APawn.h"
#include "AAsset.h"

vector<shared_ptr<APawn>> ObjectLoader::Load()
{
	vector<string> objFileNameList = GetFileNames("../Resources/Obj/*.obj");
	vector<vector<MeshData>> meshes;
	for (int iFile = 0; iFile < objFileNameList.size(); iFile++)
	{
		meshes.emplace_back(objectLoader.Load(objFileNameList[iFile]));
	}

	// MAKE TEMP MATERIAL
	shared_ptr<UMaterial> material = make_shared<UMaterial>();
	wstring path = L"../Resources/Texture/kkongchi.jpg";
	material->Load(path, L"../Resources/Shader/Default.hlsl");

	for (int iObj = 0; iObj < meshes.size(); iObj++)
	{
		shared_ptr<APawn> actor = make_shared<APawn>();

		////MESH
		shared_ptr<UStaticMeshComponent> rootMesh = make_shared<UStaticMeshComponent>();

		for (int iMesh = 0; iMesh < meshes[iObj].size(); iMesh++)
		{
			shared_ptr<UStaticMeshResources> mesh = make_shared<UStaticMeshResources>();
			mesh->SetName(meshes[iObj][iMesh].m_szName);
			mesh->SetVertexList(meshes[iObj][iMesh].m_vVertexList);
			mesh->SetIndexList(meshes[iObj][iMesh].m_vIndexList);
			mesh->Create();
			rootMesh->SetMesh(mesh);
			rootMesh->SetMaterial(material);
			m_mMeshMap.insert(make_pair(meshes[iObj][iMesh].m_szName, mesh));
		}
		m_vMeshList.emplace_back(rootMesh);
		actor->SetMeshComponent(rootMesh);
		actor->Init();
		m_vObjList.emplace_back(actor);
	}
	return m_vObjList;
}

void ObjectLoader::ConvertObjToAsset(string _path)
{
	vector<string> fileList = GetFileNames(_path);

	for (int iPath = 0; iPath < fileList.size(); iPath++)
	{
		LoadAsFR(fileList[iPath]);
		//AAsset::Export(ret, fileList[iPath]);
	}
}

vector<shared_ptr<UMeshComponent>> ObjectLoader::LoadMesh()
{
	return m_vMeshList;
}

map<wstring, shared_ptr<UMeshResources>> ObjectLoader::LoadMeshMap()
{

	return m_mMeshMap;
}

void ObjectLoader::LoadAsFR(string path)
{
	TFbxResource ret;
	ret.Clear();
	vector<MeshData> dataList = objectLoader.Load(path);
	ret.m_iMeshCount = 1;
	ret.m_vMeshList.emplace_back(dataList[0]);

	AAsset::Export(ret, path);
}
