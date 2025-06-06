#include "pch.h"
#include "Game.h"

// Manager
#include "ObjectManager.h"
#include "CameraManager.h"
#include "LightManager.h"
#include "UIManager.h"
#include "Timer.h"
#include "Input.h"
#include "PrefabToActor.h"
#include "CollisionManager.h"
#include "EffectManager.h"
#include "Sound.h"
#include "WindManager.h"

// Actor
#include "ATerrainTileActor.h"
#include "ACameraActor.h"
#include "ASky.h"
#include "ALight.h"
#include "AUIActor.h"
#include "AWindActor.h"

// Component
#include "UStaticMeshComponent.h"

// Script
#include "EngineCameraMoveScript.h"
#include "GameCameraMove.h"


// TEMP
#include "BatMovement.h"
#include "WalkerMovement.h"
#include "BettyMovement.h"
#include "Timer.h"

void Game::Init()
{
	// Asset 로딩

	m_vMapList = PToA->LoadAllPrefabs(".map.json");
	m_vObjectList = PToA->LoadAllPrefabs(".objects.json");

	OBJECT->AddActorList(m_vMapList);
	OBJECT->AddActorList(PToA->LoadAllPrefabs(".object.json"));
	OBJECT->AddActorList(m_vObjectList);
	OBJECT->AddActorList(PToA->LoadAllPrefabs(".particlegroup.json"));

	m_pPlayer = PToA->MakeCharacter("../Resources/Prefab/Player/Mycharacter.character.json");
	m_pPlayer->SetUseStencil(true);
	OBJECT->AddActor(m_pPlayer);

	auto vlist = PToA->LoadAllPrefabs(".character.json");
	OBJECT->AddActorList(vlist);
	
	// Temp
	enemyList = vlist;
	SetEnemyScript();

	// UI
	UI->AddUIList(PToA->MakeUIs("../Resources/Prefab/UI_Game_BackGround.uis.json"));
	UI->DoFadeOut();

	EFFECT->Init();
	SetupEngineCamera();
	SetupGameCamera();
	SetupSkybox();
	SetupSunLight();


	//{
	//	auto wind = std::make_shared<AWindActor>();
	//	wind->SetPosition(Vec3(0.f, 0.f, 0.95f));     // 살짝 앞쪽 Z (카메라 무시 대상)
	//	wind->SetScale(Vec3(0.5f, 0.05f, 1.f));      // 바람 길이, 너비 조절
	//	wind->SetRotation(Vec3(0, 0, 3));
	//	WIND->AddWind(wind);
	//}
}

void Game::Tick()
{
	m_pSky->AddRotation(Vec3(0.0f, 0.05f * TIMER->GetDeltaTime(), 0.0f));

	//bgm
	{
		SOUNDMANAGER->GetPtr(ESoundType::Stage0)->Play2D();
	}
	
	if (INPUT->GetButton(L))
	{
		m_bWind = !m_bWind;
	}

	if(m_bWind)
	CreateWind();
	
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
			CAMERA->Set3DCameraActor(m_pCameraActor);
		}
	}

	CheckEnemyCollision();
}

void Game::Render()
{
}

void Game::Destroy()
{
	m_pCameraActor = nullptr;
	m_pPlayer = nullptr;
	m_pSkyMesh = nullptr;
	m_pSky = nullptr;
	m_pSunLight = nullptr;
}

void Game::SetupEngineCamera()
{
	m_pCameraActor = make_shared<ACameraActor>();

	m_pCameraActor->SetPosition({ 0.0f, 10.0f, 0.0f });
	m_pCameraActor->AddScript(make_shared<EngineCameraMoveScript>());
	m_pCameraActor->m_szName = L"EnginCamera";

	CAMERA->Set3DCameraActor(m_pCameraActor);
	OBJECT->AddActor(m_pCameraActor);
}
void Game::SetupGameCamera()
{
	m_pGameCameraActor = make_shared<ACameraActor>();
	
	auto script = make_shared<GameCameraMove>(m_pPlayer);
	m_pGameCameraActor->AddScript(script);
	m_pGameCameraActor->m_szName = L"GameCamera";

	CAMERA->Set3DCameraActor(m_pGameCameraActor);
	OBJECT->AddActor(m_pGameCameraActor);

}

void Game::SetupSkybox()
{
	m_pSky = make_shared<ASky>();
	m_pSky->m_szName = L"Sky";
	m_pSkyMesh = UStaticMeshComponent::CreateSphere(20, 20);
	m_pSky->SetMeshComponent(m_pSkyMesh);

	shared_ptr<UMaterial> material = make_shared<UMaterial>();
	material->Load(L"../Resources/Texture/skypano.png", L"../Resources/Shader/Sky.hlsl");
	m_pSkyMesh->SetMaterial(material);

	OBJECT->AddActor(m_pSky);
}

void Game::SetupSunLight()
{
	m_pSunLight = make_shared<ALight>();
	m_pSunLight->m_szName = L"SunLight";
	m_pSunLight->GetLightComponent()->SetDirection({ 0, -1.f, 0 });
	m_pSunLight->GetLightComponent()->SetAmbientColor(Vec3(1.0f, 1.0f, 1.0f));
	m_pSunLight->GetLightComponent()->SetAmbientPower(0.3f);
	m_pSunLight->SetPosition(Vec3(0, 100.0f, 0));
	m_pSunLight->SetScale(Vec3(10.0f, 10.0f, 10.0f));
	OBJECT->AddActor(m_pSunLight);

	LIGHTMANAGER->Clear();
	LIGHTMANAGER->RegisterLight(m_pSunLight);
}

void Game::CreateWind()
{
	static float accTime = 0.f;
	static float randTarget = RandomRange(0.5f, 2.0f); // 처음 랜덤 타이밍

	accTime += TIMER->GetDeltaTime();

	if (accTime >= randTarget)
	{
		accTime = 0.f;
		randTarget = RandomRange(0.8f, 1.5f); // 다음 타이밍

		int spawnCount = RandomRange(2, 3);

		for (int i = 0; i < spawnCount; ++i)
		{
			// NDC 기준 왼쪽 위에서 오른쪽 아래로
			float startX = RandomRange(-1.2f, 0); // 살짝 바깥쪽에서 시작
			float startY = RandomRange(0.5f, 1.5f); // 상단에서만
			if (startY < 1.0f)
			{
				startX = -1.2f;
			}

			Vec3 pos = Vec3(startX, startY, 0.95f); // 화면 앞으로

			auto wind = std::make_shared<AWindActor>();
			wind->SetPosition(pos);
			wind->SetScale(Vec3(0.3f, 0.015f, 1.f));  // NDC 기준 적절한 크기
			wind->SetRotation(Vec3(0, 0, 2.5f));     // ↘ 방향

			WIND->AddWind(wind);
		}
		
	}
}

void Game::SetEnemyScript()
{
	for (auto& enemy : enemyList)
	{
		// 
		if (enemy->GetScriptList().size() <= 0) { continue; }
		// bat
		{
			auto script = dynamic_pointer_cast<BatMovement>(enemy->GetScriptList()[0]);
			if (script) { script->SetPlayer(m_pPlayer); }
		}

		// walker
		{
			auto script = dynamic_pointer_cast<WalkerMovement>(enemy->GetScriptList()[0]);
			if (script) { script->SetPlayer(m_pPlayer); }
		}

		// betty
		{
			auto script = dynamic_pointer_cast<BettyMovement>(enemy->GetScriptList()[0]);
			if (script) { script->SetPlayer(m_pPlayer); }
		}

	}
}

void Game::CheckEnemyCollision()
{
	for (auto iter = enemyList.begin(); iter != enemyList.end();)
	{
		if ((iter->get() == nullptr) || iter->get()->m_bDelete == true)
		{
			iter = enemyList.erase(iter);
			continue;
		}
		COLLITION->CheckCollision(m_pPlayer, *iter);
		iter++;
	}

	for (auto iter = m_vObjectList.begin(); iter != m_vObjectList.end();)
	{
		if ((iter->get() == nullptr) || iter->get()->m_bDelete == true)
		{
			iter = m_vObjectList.erase(iter);
			continue;
		}
		COLLITION->CheckCollision(m_pPlayer, *iter);
		iter++;
	}

	for (auto iter = m_vMapList.begin(); iter != m_vMapList.end();)
	{
		if ((iter->get() == nullptr) || iter->get()->m_bDelete == true)
		{
			iter = m_vMapList.erase(iter);
			continue;
		}
		COLLITION->CheckCollision(m_pPlayer, *iter);
		iter++;
	}
}
