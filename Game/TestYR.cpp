#include "pch.h"
#include "TestYR.h"
#include "ACameraActor.h"
#include "EngineCameraMoveScript.h"
#include "CameraManager.h"
#include "ActorLoader.h"
#include "MeshLoader.h"
#include "PlayerMoveScript.h"
#include "USkinnedMeshComponent.h"
#include "UStaticMeshComponent.h"
#include "UIManager.h"
#include "ObjectManager.h"
#include "LightManager.h"
#include "ALight.h"
#include "AActor.h"
#include "APawn.h"
#include "UMaterial.h"

#include "Input.h"
#include "AnimTrack.h"
#include "AAsset.h"

#include "PrefabToActor.h"
#include "GameCameraMove.h"

#include "BatMovement.h"
#include "CollisionManager.h"

void TestYR::Init()
{
#pragma region Set Character
	auto vlist = PToA->LoadAllPrefabs(".character.json");
	player = PToA->MakeCharacter("../Resources/Prefab/Player/Mycharacter.character.json");
	OBJECT->AddActor(player);
	enemyList = vlist;
	for (auto& enemy : vlist)
	{
		if (enemy->GetScriptList().size() <= 0) { continue; }
		dynamic_pointer_cast<BatMovement>(enemy->GetScriptList()[0])->SetPlayer(player);
	}
	OBJECT->AddActorList(vlist);

#pragma endregion
#pragma region SetupScene
	SetupGameCamera();
	SetupEngineCamera();
	SetupSunLight();
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
		shared_ptr<UStaticMeshComponent> meshComponent2 = UStaticMeshComponent::CreateCube();
		shared_ptr<UStaticMeshComponent> meshComponent3 = UStaticMeshComponent::CreateCube();

		meshComponent1->SetMaterial(material);
		meshComponent2->SetMaterial(material);
		meshComponent3->SetMaterial(material);

		// Object 汲沥
		object1->SetMeshComponent(meshComponent1);
		object2->SetMeshComponent(meshComponent2);
		object3->SetMeshComponent(meshComponent3);

		object1->SetScale(Vec3(1000.0f, 0.03f, 0.03f));
		object2->SetScale(Vec3(0.03f, 1000.0f, 0.03f));
		object3->SetScale(Vec3(0.03f, 0.03f, 1000.0f));

		OBJECT->AddActor(object1);
		OBJECT->AddActor(object2);
		OBJECT->AddActor(object3);


		gizmo.emplace_back(object1);
		gizmo.emplace_back(object2);
		gizmo.emplace_back(object3);
	}
#pragma endregion
}
void TestYR::Tick()
{
	if (INPUT->GetButton(O))
	{
		if (m_bEnginCamera)
		{
			m_bEnginCamera = false;
			CAMERA->Set3DCameraActor(m_pGameCameraActor);
		}
		else
		{
			m_bEnginCamera = true;
			CAMERA->Set3DCameraActor(m_pCameraEngine);
		}
	}

	CheckEnemyCollision();
}

void TestYR::Render()
{
}

void TestYR::Destroy()
{
}

void TestYR::SetupSunLight()
{
	auto m_pSunLight = make_shared<ALight>();
	m_pSunLight->GetLightComponent()->SetDirection({ 0, -1.f, 0 });
	m_pSunLight->GetLightComponent()->SetAmbientColor(Vec3(1.0f, 1.0f, 1.0f));
	m_pSunLight->GetLightComponent()->SetAmbientPower(0.3f);
	m_pSunLight->SetPosition(Vec3(0, 100.0f, 0));
	m_pSunLight->SetScale(Vec3(10.0f, 10.0f, 10.0f));
	OBJECT->AddActor(m_pSunLight);

	LIGHTMANAGER->Clear();
	LIGHTMANAGER->RegisterLight(m_pSunLight);
}

void TestYR::SetupEngineCamera()
{
	m_pCameraEngine = make_shared<ACameraActor>();

	m_pCameraEngine->SetPosition({ 0.0f, 10.0f, -10.0f });
	m_pCameraEngine->AddScript(make_shared<EngineCameraMoveScript>());
	m_pCameraEngine->m_szName = L"EnginCamera";

	CAMERA->Set3DCameraActor(m_pCameraEngine);
	OBJECT->AddActor(m_pCameraEngine);
}


void TestYR::SetupGameCamera()
{
	m_pGameCameraActor = make_shared<ACameraActor>();

	//m_pGameCameraActor->SetPosition({ 0.0f, 10.0f, 0.0f });
	auto script = make_shared<GameCameraMove>(player);
	m_pGameCameraActor->AddScript(script);
	m_pGameCameraActor->m_szName = L"GameCamera";

	CAMERA->Set3DCameraActor(m_pGameCameraActor);
	OBJECT->AddActor(m_pGameCameraActor);

}

void TestYR::CheckEnemyCollision()
{
	for (auto iter = enemyList.begin(); iter != enemyList.end();)
	{
		if ((iter->get() == nullptr) || iter->get()->m_bDelete == true)
		{
			iter = enemyList.erase(iter);
			continue;
		}
		COLLITION->CheckCollision(player, *iter);
		iter++;
	}
}
