#include "pch.h"
#include "TestYR.h"
#include "ACameraActor.h"
#include "EngineCameraMoveScript.h"
#include "UStaticMeshComponent.h"
#include "Input.h"
#include "UMaterial.h"

void TestYR::Init()
{

	//Gizmo 技泼
	{
		shared_ptr<APawn> object1 = make_shared<APawn>();
		shared_ptr<APawn> object2 = make_shared<APawn>();
		shared_ptr<APawn> object3 = make_shared<APawn>();

		// Material 积己
		shared_ptr<UMaterial> material = make_shared<UMaterial>();
		material->Load(L"../Resources/Texture/kkongchi.jpg", L"../Resources/Shader/Default.hlsl");

		// mesh 积己
		shared_ptr<UStaticMeshComponent> meshComponent1 = UStaticMeshComponent::CreateCube();
		meshComponent1->SetMaterial(material);
	
		// Object 汲沥
		object1->SetMesh(meshComponent1);
		object2->SetMesh(meshComponent1);
		object3->SetMesh(meshComponent1);

		object1->SetScale(Vec3(10000.0f, 0.03f, 0.03f));
		object2->SetScale(Vec3(0.03f, 10000.0f, 0.03f));
		object3->SetScale(Vec3(0.03f, 0.03f, 10000.0f));
		
		object1->Init();
		object2->Init();
		object3->Init();
		
		gizmo.emplace_back(object1);
		gizmo.emplace_back(object2);
		gizmo.emplace_back(object3);
	}

	m_pCameraActor = make_shared<ACameraActor>();
	{
		m_pCameraActor->SetPosition({ 0.0f, 0.0f, 0.0f });
		m_pCameraActor->AddScript(make_shared<EngineCameraMoveScript>());
	}
	m_pCameraActor->Init();

	m_vActorList = loader.Load();
	for (int i = 0; i < m_vActorList.size(); i++)
	{
		m_vActorList[i]->Init();
	}

	// LOAD ALL TEXTURE
	{
		string path = "../Resources/Texture/ObjTexture/*.png";
		HANDLE hFind;
		WIN32_FIND_DATAA data;
		vector<string> fileList;
		if ((hFind = FindFirstFileA(path.c_str(), &data)) != INVALID_HANDLE_VALUE)
		{
			do
			{
				string directory = "../Resources/Texture/ObjTexture/";
				directory += string(data.cFileName);
				fileList.emplace_back(directory);
			} while (FindNextFileA(hFind, &data) != 0);
			FindClose(hFind);
		}

		for (int iPath = 0; iPath < fileList.size(); iPath++)
		{
			shared_ptr<UMaterial> tempMat = make_shared<UMaterial>();

			tempMat->Load(to_mw(fileList[iPath]), L"../Resources/Shader/Default.hlsl");
			materialList.emplace_back(tempMat);
		}

	}

	MakeObject();

	targetObj = m_vObjList[targetIndex];
	targetObj->GetMesh<UStaticMeshComponent>()->SetMaterial(materialList[matIndex]);
}
void TestYR::Update()
{
	m_pCameraActor->Tick();
	for (int i = 0; i < gizmo.size(); i++)
	{
		gizmo[i]->Tick();
	}
	//for (int i = 0; i < m_vObjList.size(); i++)
	//{
	//	m_vObjList[i]->Tick();
	//}
	targetObj->Tick();

	if (INPUT->GetButton(GameKey::C))
	{
		if (++targetIndex >= m_vObjList.size())
		{
			targetIndex = 0;
		}
		targetObj = m_vObjList[targetIndex];
	}

	if (INPUT->GetButton(GameKey::T))
	{
		if (++matIndex >= materialList.size())
		{
			matIndex = 0;
		}
		targetObj->GetMesh<UStaticMeshComponent>()->SetMaterial(materialList[matIndex]);
	}

	if (INPUT->GetButton(GameKey::Y))
	{
		if (--matIndex < 0)
		{
			matIndex = 0;
		}
		targetObj->GetMesh<UStaticMeshComponent>()->SetMaterial(materialList[matIndex]);
	}
}
void TestYR::Render()
{
	m_pCameraActor->Render();
	for (int i = 0; i < gizmo.size(); i++)
	{
		gizmo[i]->Render();
	}
	//for (int i = 0; i < m_vActorList.size(); i++)
	//{
	//	m_vActorList[i]->Render();
	//}

	//for (int i = 0; i < m_vObjList.size(); i++)
	//{
	//	m_vObjList[i]->Render();
	//}
	targetObj->Render();
}

void TestYR::MakeObject()
{
	vector<string> objFileNameList = GetFileNames("../Resources/Obj/*.obj");
	vector<vector<MeshData>> meshes;
	for (int iFile = 0; iFile < objFileNameList.size(); iFile++)
	{
		meshes.emplace_back(objectLoader.Load(objFileNameList[iFile]));
	}

	// MAKE TEMP MATERIAL
	shared_ptr<UMaterial> material = make_shared<UMaterial>();
	wstring path = L"../Resources/Texture/magenta.png";
	material->Load(path, L"../Resources/Shader/Default.hlsl");

	for (int iObj = 0; iObj < meshes.size(); iObj++)
	{
		shared_ptr<APawn> actor = make_shared<APawn>();

		////MESH
		// TEMP ( CHILD 贸府 固软 ) 
		shared_ptr<UStaticMeshComponent> rootMesh = make_shared<UStaticMeshComponent>();

		for (int iMesh = 0; iMesh < meshes[iObj].size(); iMesh++)
		{
			shared_ptr<UStaticMeshResources> mesh = make_shared<UStaticMeshResources>();
			mesh->SetVertexList(meshes[iObj][iMesh].m_vVertexList);
			mesh->SetIndexList(meshes[iObj][iMesh].m_vIndexList);
			mesh->Bind();
			rootMesh->SetMesh(mesh);
			rootMesh->SetMaterial(material);
		}

		actor->SetMesh(rootMesh);
		actor->Init();
		m_vObjList.emplace_back(actor);
	}

}
