#include "pch.h"
#include "ObjectLoader.h"
#include "UMaterial.h"
#include "UStaticMeshComponent.h"
#include "APawn.h"

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
		// TEMP ( CHILD √≥∏Æ πÃ»Ì ) 
		shared_ptr<UStaticMeshComponent> rootMesh = make_shared<UStaticMeshComponent>();

		for (int iMesh = 0; iMesh < meshes[iObj].size(); iMesh++)
		{
			shared_ptr<UStaticMeshResources> mesh = make_shared<UStaticMeshResources>();
			mesh->SetVertexList(meshes[iObj][iMesh].m_vVertexList);
			mesh->SetIndexList(meshes[iObj][iMesh].m_vIndexList);
			mesh->Create();
			rootMesh->SetMesh(mesh);
			rootMesh->SetMaterial(material);
		}
		m_vMeshList.emplace_back(rootMesh);
		actor->SetMeshComponent(rootMesh);
		actor->Init();
		m_vObjList.emplace_back(actor);
	}
	return m_vObjList;
}

vector<shared_ptr<UMeshComponent>> ObjectLoader::LoadMesh()
{
	return m_vMeshList;
}
