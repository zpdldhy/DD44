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

	CAMERAMANAGER->SetCameraActor(m_pCameraActor);

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

	GUI->SetObjectEditorCallback([this](int actorType, int meshType, const char* texPath, const char* shaderPath, const char* objPath, Vec3 pos, Vec3 rot, Vec3 scale)
		{
			AssimpLoader loader;
			//vector<MeshData> meshList = loader.Load(objPath);
			if (meshList.empty())
				return;

			// Mesh Component 생성
			shared_ptr<UMeshComponent> meshComp = meshList[meshType];

			// 머티리얼 설정
			auto mat = make_shared<UMaterial>();
			mat->Load(
				std::wstring(texPath, texPath + strlen(texPath)),
				std::wstring(shaderPath, shaderPath + strlen(shaderPath))
			);

			if (meshType == 0)
			{
				// Animation 세팅
				shared_ptr<AnimTrack> animTrack = make_shared< AnimTrack>();
				animTrack->SetBase(animInstanceList[0]);
				dynamic_cast<USkinnedMeshComponent*>(meshComp.get())->SetBaseAnim(animInstanceList[0]);
				dynamic_cast<USkinnedMeshComponent*>(meshComp.get())->SetMeshAnim(animTrack);
				
				// Material INPUTLAYOUT 
				mat->SetInputlayout(INPUTLAYOUT->Get(L"IW"));
			}

			meshComp->SetMaterial(mat);

			// Actor 생성
			shared_ptr<AActor> actor;
			if (actorType == 0)
			{
				actor = make_shared<APawn>(); // ACharacter로 교체 가능
			}
			else if (actorType == 1)
			{
				actor = make_shared<APawn>(); // AEnemy로 교체 가능
			}
			else if (actorType == 2)
			{
				actor = make_shared<APawn>(); // AObject로 교체 가능
			}

			actor->SetMeshComponent(meshComp);

			//// 타일이 있다면 위치 보정
			//if (!m_vTiles.empty())
			//{
			//	float y = m_vTiles.back()->GetHeightAt(pos.x, pos.z);
			//	pos.y = y + scale.y / 2.0f;
			//}


			actor->SetPosition(pos);
			actor->SetRotation(rot);
			actor->SetScale(scale);
			actor->Init();

			m_vImGuiList.push_back(actor);
		});



}
void TestYR::Update()
{
	m_pCameraActor->Tick();
	for (int i = 0; i < gizmo.size(); i++)
	{
		gizmo[i]->Tick();
	}

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


