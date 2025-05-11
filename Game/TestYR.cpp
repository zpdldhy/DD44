#include "pch.h"
#include "TestYR.h"
#include "ACameraActor.h"
#include "EngineCameraMoveScript.h"
#include "CameraManager.h"
#include "UStaticMeshComponent.h"
#include "USkinnedMeshComponent.h"
#include "Input.h"
#include "UMaterial.h"
#include "UAnimInstance.h"
#include "AnimTrack.h"
#include "ImGuiCore.h"
#include "UIManager.h"
#include "ObjectManager.h"

void TestYR::Init()
{
	//Gizmo 세팅
	{
		shared_ptr<APawn> object1 = make_shared<APawn>();
		shared_ptr<APawn> object2 = make_shared<APawn>();
		shared_ptr<APawn> object3 = make_shared<APawn>();

		// Material 생성
		shared_ptr<UMaterial> material = make_shared<UMaterial>();
		material->Load(L"../Resources/Texture/kkongchi.jpg", L"../Resources/Shader/Default.hlsl");

		// mesh 생성
		shared_ptr<UStaticMeshComponent> meshComponent1 = UStaticMeshComponent::CreateCube();
		meshComponent1->SetMaterial(material);

		// Object 설정
		object1->SetMeshComponent(meshComponent1);
		object2->SetMeshComponent(meshComponent1);
		object3->SetMeshComponent(meshComponent1);

		object1->SetScale(Vec3(10000.0f, 0.03f, 0.03f));
		object2->SetScale(Vec3(0.03f, 10000.0f, 0.03f));
		object3->SetScale(Vec3(0.03f, 0.03f, 10000.0f));		

		gizmo.emplace_back(object1);
		gizmo.emplace_back(object2);
		gizmo.emplace_back(object3);

		OBJECTMANAGER->AddActorList(gizmo);
	}

	m_pCameraActor = make_shared<ACameraActor>();
	{
		m_pCameraActor->SetPosition({ 0.0f, 0.0f, 0.0f });
		m_pCameraActor->AddScript(make_shared<EngineCameraMoveScript>());
	}
	OBJECTMANAGER->AddActor(m_pCameraActor);

	//loader.ConvertFbxToAsset();
	/*m_vActorList = loader.Load();
	meshList = loader.LoadMesh();
	texPathList = loader.LoadTexPath();
	animInstanceList = loader.LoadAnim();*/

	loader.LoadOne("../Resources/Asset/crow_final.asset");
	tempmeshList = loader.LoadMesh();



	for (int i = 0; i < m_vActorList.size(); i++)
	{
		m_vActorList[i]->Init();
		//if (i == 0)
		//{
		//	auto animInstance = m_vActorList[i]->GetMeshComponent<USkinnedMeshComponent>()->GetAnimInstance();
		//	animInstance->CheckInPlace(true);
		//	int rootIndex = m_vActorList[i]->GetMeshComponent<USkinnedMeshComponent>()->GetMesh()->GetBoneIndex(L"_RollRoot");
		//	animInstance->SetRootIndex(rootIndex);
		//}
		//if (i == 1)
		//{
		//	auto animInstance = m_vActorList[i]->GetMeshComponent<USkinnedMeshComponent>()->GetAnimInstance();
		//	animInstance->CheckInPlace(true);
		//	//int rootIndex = m_vActorList[i]->GetMeshComponent<USkinnedMeshComponent>()->GetMesh()->GetBoneIndex(L"_RollRoot");
		//	animInstance->SetRootIndex(0);
		//}
		//auto animInstance = m_vActorList[i]->GetMeshComponent<USkinnedMeshComponent>()->GetAnimInstance();
		//animInstance->CheckInPlace(true);
		//int rootIndex = m_vActorList[i]->GetMeshComponent<USkinnedMeshComponent>()->GetMesh()->GetBoneIndex(L"_RollRoot");
		//animInstance->SetRootIndex(rootIndex);
	}

	CAMERAMANAGER->Set3DCameraActor(m_pCameraActor);

	// LOAD ALL TEXTURE
	{
		//string path = "../Resources/Texture/ObjTexture/*.png";
		//HANDLE hFind;
		//WIN32_FIND_DATAA data;
		//vector<string> fileList;
		//if ((hFind = FindFirstFileA(path.c_str(), &data)) != INVALID_HANDLE_VALUE)
		//{
		//	do
		//	{
		//		string directory = "../Resources/Texture/ObjTexture/";
		//		directory += string(data.cFileName);
		//		fileList.emplace_back(directory);
		//	} while (FindNextFileA(hFind, &data) != 0);
		//	FindClose(hFind);
		//}
		//for (int iPath = 0; iPath < fileList.size(); iPath++)
		//{
		//	shared_ptr<UMaterial> tempMat = make_shared<UMaterial>();
		//	tempMat->Load(to_mw(fileList[iPath]), L"../Resources/Shader/Default.hlsl");
		//	materialList.emplace_back(tempMat);
		//}
	}
	//m_vObjList = objLoader.Load();
	//vector<shared_ptr<UMeshComponent>> objMesh = objLoader.LoadMesh();
	//meshList.insert(meshList.end(), objMesh.begin(), objMesh.end());
	//m_vObjList[0]->SetScale(Vec3(12.0f, 12.0f, 12.0f));

	//targetObj = m_vActorList[targetIndex];

	GUI->SetObjectEditorCallback([this](const char* texPath, const char* shaderPath, const char* objPath, Vec3 pos, Vec3 rot, Vec3 scale)
		{
			AssimpLoader loader;
			vector<MeshData> meshList = loader.Load(objPath);
			if (meshList.empty())
				return;

			auto meshComp = make_shared<UStaticMeshComponent>();

			auto meshRes = make_shared<UStaticMeshResources>();
			meshRes->SetVertexList(meshList[0].m_vVertexList);
			meshRes->SetIndexList(meshList[0].m_vIndexList);
			meshRes->Create();

			meshComp->SetMesh(meshRes);

			auto mat = make_shared<UMaterial>();
			mat->Load(
				std::wstring(texPath, texPath + strlen(texPath)),
				std::wstring(shaderPath, shaderPath + strlen(shaderPath))
			);
			meshComp->SetMaterial(mat);

			// Snap 적용 여부 확인
			if (GUI->GetObjectEditorUI()->IsSnapEnabled())
			{
				pos = GUI->GetObjectEditorUI()->SnapToGrid(pos, 1.0f);
			}

			auto actor = make_shared<APawn>();
			actor->SetMeshComponent(meshComp);
			actor->SetPosition(pos);
			actor->SetRotation(rot);
			actor->SetScale(scale);

			OBJECTMANAGER->AddActor(actor);
		});
}
void TestYR::Update()
{
	//if (targetObj)
	//{
	//	targetObj->Tick();
	//}

	for (auto& actor : m_vImGuiList)
	{
		actor->Tick();
	}

	//if (INPUT->GetButton(GameKey::C))
	//{
	//	if (++targetIndex >= m_vActorList.size())
	//	{
	//		targetIndex = 0;
	//	}
	//	targetObj = m_vActorList[targetIndex];
	//}

	//if (INPUT->GetButton(GameKey::R))
	//{
	//	
	//	auto m_pAnimInstance = dynamic_cast<USkinnedMeshComponent*>(targetObj->GetMeshComponent<USkinnedMeshComponent>().get())->GetAnimInstance();
	//	int targetIndex = m_pAnimInstance->GetAnimIndex(L"Arrow_bomb");
	//	m_pAnimInstance->SetCurrentAnimTrack(targetIndex);
	//}

	// Texture 바꾸기
	{
		//if (INPUT->GetButton(GameKey::T))
		//{
		//	if (++matIndex >= materialList.size())
		//	{
		//		matIndex = 0;
		//	}
		//	targetObj->GetMeshComponent<UStaticMeshComponent>()->SetMaterial(materialList[matIndex]);
		//}

		//if (INPUT->GetButton(GameKey::Y))
		//{
		//	if (--matIndex < 0)
		//	{
		//		matIndex = 0;
		//	}
		//	targetObj->GetMeshComponent<UStaticMeshComponent>()->SetMaterial(materialList[matIndex]);
		//}
	}
}
void TestYR::Render()
{
	for (int i = 0; i < gizmo.size(); i++)
	{
		gizmo[i]->Render();
	}

	/*targetObj->Render();*/

	for (auto& actor : m_vImGuiList)
	{
		actor->Render();
	}
}


