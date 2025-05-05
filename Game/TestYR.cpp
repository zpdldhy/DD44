#include "pch.h"
#include "TestYR.h"
#include "ACameraActor.h"
#include "EngineCameraMoveScript.h"
#include "CameraManager.h"
#include "UStaticMeshComponent.h"
#include "USkinnedMeshComponent.h"
#include "Input.h"
#include "UMaterial.h"
#include "AnimTrack.h"
#include "ImGuiCore.h"

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

	//loader.ConvertFbxToAsset();
	m_vActorList = loader.Load();
	meshList = loader.LoadMesh();
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

	CAMERAMANAGER->SetCameraActor(m_pCameraActor);

	//// LOAD ALL TEXTURE
	//{
	//	string path = "../Resources/Texture/ObjTexture/*.png";
	//	HANDLE hFind;
	//	WIN32_FIND_DATAA data;
	//	vector<string> fileList;
	//	if ((hFind = FindFirstFileA(path.c_str(), &data)) != INVALID_HANDLE_VALUE)
	//	{
	//		do
	//		{
	//			string directory = "../Resources/Texture/ObjTexture/";
	//			directory += string(data.cFileName);
	//			fileList.emplace_back(directory);
	//		} while (FindNextFileA(hFind, &data) != 0);
	//		FindClose(hFind);
	//	}
	//	for (int iPath = 0; iPath < fileList.size(); iPath++)
	//	{
	//		shared_ptr<UMaterial> tempMat = make_shared<UMaterial>();
	//		tempMat->Load(to_mw(fileList[iPath]), L"../Resources/Shader/Default.hlsl");
	//		materialList.emplace_back(tempMat);
	//	}
	//}
	//m_vObjList = objLoader.Load();

	targetObj = m_vActorList[targetIndex];

	GUI->InitMeshDataByCallBack(m_vActorList.size(), meshList.size());



	GUI->SetObjectCallback([this](int target)
		{
			targetIndex = target;
			targetObj = m_vActorList[targetIndex];
		}
	);

	GUI->SetMeshCallback([this](int targetActor, int targetMesh)
		{
			m_vActorList[targetActor]->GetMeshComponent<USkinnedMeshComponent>()->AddChild(meshList[targetMesh]);
		}
	);
}
void TestYR::Update()
{
	m_pCameraActor->Tick();
	for (int i = 0; i < gizmo.size(); i++)
	{
		gizmo[i]->Tick();
	}

	targetObj->Tick();

	if (INPUT->GetButton(GameKey::C))
	{
		if (++targetIndex >= m_vActorList.size())
		{
			targetIndex = 0;
		}
		targetObj = m_vActorList[targetIndex];
	}

	// Texture 바꾸기
	{
		/*if (INPUT->GetButton(GameKey::T))
		{
			if (++matIndex >= materialList.size())
			{
				matIndex = 0;
			}
			targetObj->GetMeshComponent<UStaticMeshComponent>()->SetMaterial(materialList[matIndex]);
		}

		if (INPUT->GetButton(GameKey::Y))
		{
			if (--matIndex < 0)
			{
				matIndex = 0;
			}
			targetObj->GetMeshComponent<UStaticMeshComponent>()->SetMaterial(materialList[matIndex]);
		}*/
	}
}
void TestYR::Render()
{
	for (int i = 0; i < gizmo.size(); i++)
	{
		gizmo[i]->Render();
	}

	targetObj->Render();
}


