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
#include "WalkerMovement.h"
#include "BettyMovement.h"
#include "MageMovement.h"
#include "CollisionManager.h"
#include "UBoxComponent.h"
#include "EnemyCollisionManager.h"
#include "EffectManager.h"
#include "ProjectileManager.h"

void TestYR::Init()
{
#pragma region Set Character
	PToA->Init();
	auto vlist = PToA->LoadAllPrefabs(".character.json");
	player = PToA->MakeCharacter("../Resources/Prefab/Player/Mycharacter.character.json");
	player->SetPosition(Vec3(0.0f, 0.0f, 0.0f));
	OBJECT->AddActor(player);

	//m_vObjectList = PToA->LoadAllPrefabs(".objects.json");
	//OBJECT->AddActorList(m_vObjectList);

	boss_betty = PToA->MakeCharacter("../Resources/Prefab/Player/Boss_Betty_test.character.json");
	//OBJECT->AddActor(boss_betty);

	vlist.emplace_back(boss_betty);
	enemyList = vlist;
	for (auto& enemy : vlist)
	{
		if (enemy->GetScriptList().size() <= 0) { continue; }
		auto bat = dynamic_pointer_cast<BatMovement>(enemy->GetScriptList()[0]);
		if (bat)
		{
			bat->SetPlayer(player);
		}
		auto walker = dynamic_pointer_cast<WalkerMovement>(enemy->GetScriptList()[0]);
		if (walker)
		{
			walker->SetPlayer(player);
		}
		auto betty = dynamic_pointer_cast<BettyMovement>(enemy->GetScriptList()[0]);
		if (betty)
		{
			betty->SetPlayer(player);
		}
		auto mage = dynamic_pointer_cast<MageMovement>(enemy->GetScriptList()[0]);
		if (mage)
		{
			mage->SetPlayer(player);
		}
	}
	OBJECT->AddActorList(vlist);

#pragma endregion
#pragma region SetupScene
	SetupGameCamera();
	SetupEngineCamera();
	SetupSunLight();
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
		shared_ptr<UStaticMeshComponent> meshComponent2 = UStaticMeshComponent::CreateCube();
		shared_ptr<UStaticMeshComponent> meshComponent3 = UStaticMeshComponent::CreateCube();

		meshComponent1->SetMaterial(material);
		meshComponent2->SetMaterial(material);
		meshComponent3->SetMaterial(material);

		// Object 설정
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
	EFFECT->Init();
#pragma endregion
#pragma region plain
	plain = make_shared<AActor>();
	plain->SetMeshComponent(UStaticMeshComponent::CreateCube());
	plain->SetScale(Vec3(500.f, 0.5f, 500.0f));
	auto mat = make_shared<UMaterial>();
	mat->Load(L"../Resources/Texture/Stone_SmoothGreenish_Tile.png", L"../Resources/Shader/Default.hlsl");
	plain->GetMeshComponent()->SetMaterial(mat);
	auto collider = make_shared<UBoxComponent>();
	collider->SetCollisionEnabled(CollisionEnabled::CE_QUERYANDPHYSICS);
	plain->m_szName = L"Stair";
	plain->SetShapeComponent(collider);
	m_vObjectList.emplace_back(plain);
	OBJECT->AddActor(plain);
#pragma endregion
#pragma region wall
	{
		auto wall = make_shared<AActor>();
		wall->SetMeshComponent(UStaticMeshComponent::CreateCube());
		wall->SetPosition(Vec3(0.0f, 5.0f, -35.0f));
		wall->SetScale(Vec3(70.f, 20.0f, 5.0f));
		auto mat2 = make_shared<UMaterial>();
		mat2->Load(L"../Resources/Texture/kkongchi.jpg", L"../Resources/Shader/Default.hlsl");
		wall->GetMeshComponent()->SetMaterial(mat2);
		auto collider2 = make_shared<UBoxComponent>();
		collider2->SetCollisionEnabled(CollisionEnabled::CE_QUERYANDPHYSICS);
		wall->m_szName = L"Object";
		wall->SetShapeComponent(collider2);
		m_vObjectList.emplace_back(wall);
		OBJECT->AddActor(wall); 
	}

	{
		auto wall = make_shared<AActor>();
		wall->SetMeshComponent(UStaticMeshComponent::CreateCube());
		wall->SetPosition(Vec3(0.0f, 5.0f, 35.0f));
		wall->SetScale(Vec3(70.f, 20.0f, 5.0f));
		auto mat2 = make_shared<UMaterial>();
		mat2->Load(L"../Resources/Texture/kkongchi.jpg", L"../Resources/Shader/Default.hlsl");
		wall->GetMeshComponent()->SetMaterial(mat2);
		auto collider2 = make_shared<UBoxComponent>();
		collider2->SetCollisionEnabled(CollisionEnabled::CE_QUERYANDPHYSICS);
		wall->m_szName = L"Object";
		wall->SetShapeComponent(collider2);
		m_vObjectList.emplace_back(wall);
		OBJECT->AddActor(wall);
	}

	{
		auto wall = make_shared<AActor>();
		wall->SetMeshComponent(UStaticMeshComponent::CreateCube());
		wall->SetPosition(Vec3(30.0f, 5.0f, 0.0f));
		wall->SetScale(Vec3(5.f, 20.0f, 70.0f));
		auto mat2 = make_shared<UMaterial>();
		mat2->Load(L"../Resources/Texture/kkongchi.jpg", L"../Resources/Shader/Default.hlsl");
		wall->GetMeshComponent()->SetMaterial(mat2);
		auto collider2 = make_shared<UBoxComponent>();
		collider2->SetCollisionEnabled(CollisionEnabled::CE_QUERYANDPHYSICS);
		wall->m_szName = L"Object";
		wall->SetShapeComponent(collider2);
		m_vObjectList.emplace_back(wall);
		OBJECT->AddActor(wall);
	}

	{
		auto wall = make_shared<AActor>();
		wall->SetMeshComponent(UStaticMeshComponent::CreateCube());
		wall->SetPosition(Vec3(-30.0f, 5.0f, 0.0f));
		wall->SetScale(Vec3(70.f, 20.0f, 5.0f));
		wall->SetRotation(Vec3(0.0f, DD_PI/2, 0.0f));
		auto mat2 = make_shared<UMaterial>();
		mat2->Load(L"../Resources/Texture/kkongchi.jpg", L"../Resources/Shader/Default.hlsl");
		wall->GetMeshComponent()->SetMaterial(mat2);
		auto collider2 = make_shared<UBoxComponent>();
		collider2->SetCollisionEnabled(CollisionEnabled::CE_QUERYANDPHYSICS);
		wall->m_szName = L"Object";
		wall->SetShapeComponent(collider2);
		m_vObjectList.emplace_back(wall);
		OBJECT->AddActor(wall);
	}

	
#pragma endregion

	PROJECTILE->Init();
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
	PROJECTILE->Tick();
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
	shared_ptr<AActor> melee;
	for (auto iter = ENEMYCOLLIDER->enemyList.begin(); iter != ENEMYCOLLIDER->enemyList.end();)
	{
		auto size = ENEMYCOLLIDER->enemyList.size();
		if ((iter->get() == nullptr) || iter->get()->m_bDelete == true)
		{
			iter = ENEMYCOLLIDER->enemyList.erase(iter);
			continue;
		}
		COLLITION->CheckCollision(player, *iter);
		if ((*iter)->m_szName == L"Melee")
		{
			if ((*iter)->m_bCollision)
			{
				melee = (*iter);
			}
		}
		// player melee랑 enemy 확인
		iter++;
	}

	if (melee)
	{
		for (auto iter = enemyList.begin(); iter != enemyList.end();)
		{
			if ((iter->get() == nullptr) || iter->get()->m_bDelete == true)
			{
				iter = enemyList.erase(iter);
				continue;
			}
			COLLITION->CheckCollision(*iter, melee);
			iter++;
		}
	}

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

	for (auto iter = m_vObjectList.begin(); iter != m_vObjectList.end();)
	{
		if ((iter->get() == nullptr) || iter->get()->m_bDelete == true)
		{
			iter = m_vObjectList.erase(iter);
			continue;
		}
		COLLITION->CheckCollision(player, *iter);
		COLLITION->CheckCollision(boss_betty, *iter);
		iter++;
	}

	auto list = PROJECTILE->GetActorList();
	for (auto proj = list.begin(); proj != list.end(); )
	{
		// box to sphere가 없어서 player랑은 안됨
		COLLITION->CheckCollision(*proj, player);

		for (auto iter = m_vObjectList.begin(); iter != m_vObjectList.end();)
		{
			if ((iter->get() == nullptr) || iter->get()->m_bDelete == true)
			{
				iter = m_vObjectList.erase(iter);
				continue;
			}
			COLLITION->CheckCollision(*proj, *iter);
			iter++;
		}

		for (auto iter = enemyList.begin(); iter != enemyList.end();)
		{
			if ((iter->get() == nullptr) || iter->get()->m_bDelete == true)
			{
				iter = enemyList.erase(iter);
				continue;
			}
			COLLITION->CheckCollision(*proj, *iter);
			iter++;
		}
		proj++;
	}
}