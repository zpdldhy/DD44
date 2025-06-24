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
#include "Engine.h"
#include "QuadTree.h"

// Actor
#include "ATerrainTileActor.h"
#include "ACameraActor.h"
#include "ASky.h"
#include "ALight.h"
#include "AUIActor.h"
#include "AWindActor.h"
#include "TEnemy.h"
#include "TPlayer.h"

// Component
#include "UStaticMeshComponent.h"
#include "UBoxComponent.h"

// Script
#include "ScriptManager.h"
#include "EngineCameraMoveScript.h"
#include "GameCameraMove.h"
#include "MageMovement.h"

// Game
#include "ProjectileManager.h"
#include "EnemyCollisionManager.h"
#include "EventManager.h"
#include "StageManager.h"

// TEMP
#include "BatMovement.h"
#include "WalkerMovement.h"
#include "BettyMovement.h"
#include "Timer.h"

void Game::Init()
{
	SCRIPT->Init();
	STAGE->Init();
	// Asset 로딩
	PToA->Init();
	m_vMapList = PToA->LoadAllPrefabs(".map.json");
	m_vObjectList = PToA->LoadAllPrefabs(".objects.json");
	OBJECT->AddActorList(m_vMapList);
	OBJECT->AddActorList(m_vObjectList);
	OBJECT->AddActorList(PToA->LoadAllPrefabs(".particlegroup.json"));

	m_pPlayer = PToA->MakeCharacter("../Resources/Prefab/Player/Mycharacter.character.json");
	m_pPlayer->SetUseStencil(true);
	m_pPlayer->SetPosition(Vec3(-90, 39, 70));
	//m_pPlayer->SetPosition(Vec3(10, 0, 0));
	//m_pPlayer->SetPosition(Vec3(-63, 28, 26));
	//m_pPlayer->SetPosition(Vec3(78.6, -0.32, -100));
	OBJECT->AddActor(m_pPlayer);

	auto objectList = PToA->LoadAllPrefabs(".character.json");
	OBJECT->AddActorList(objectList);
	for (auto interactable : objectList)
	{
		m_vObjectList.push_back(interactable);
	}

	m_pBetty = PToA->MakeCharacter("../Resources/Prefab/Player/Boss_Betty.character.json");
	enemyList.emplace_back(m_pBetty);
	//tempHeadRoller = PToA->MakeCharacter("../Resources/Prefab/Player/HeadRoller1.character.json");
	//enemyList.emplace_back(tempHeadRoller);

	auto enemy00 = PToA->LoadAllPrefabs(".character.json", "../Resources/Prefab/Stage00/");
	auto enemy01 = PToA->LoadAllPrefabs(".character.json", "../Resources/Prefab/Stage01/");
	auto enemy02 = PToA->LoadAllPrefabs(".character.json", "../Resources/Prefab/Stage02/");
	auto enemy10 = PToA->LoadAllPrefabs(".character.json", "../Resources/Prefab/Stage10/");
	auto enemy20 = PToA->LoadAllPrefabs(".character.json", "../Resources/Prefab/Stage20/");
	auto otherEnemy = PToA->LoadAllPrefabs(".character.json", "../Resources/Prefab/Other/");

	for (auto e : enemy00)
	{
		enemyList.emplace_back(e);
	}

	for (auto e : enemy01)
	{
		enemyList.emplace_back(e);
	}

	for (auto e : enemy02)
	{
		enemyList.emplace_back(e);
	}
	for (auto e : enemy10)
	{
		enemyList.emplace_back(e);
	}
	for (auto e : enemy20)
	{
		enemyList.emplace_back(e);
	}
	for (auto e : otherEnemy)
	{
		enemyList.emplace_back(e);
	}

	SetEnemy(enemyList);
	OBJECT->AddActorList(enemyList);
	STAGE->AddEnemiesInStage(0, enemy00);
	STAGE->AddEnemiesInStage(1, enemy01);
	STAGE->AddEnemiesInStage(2, enemy10);

	PROJECTILE->Init();

	CreateUI();

	EFFECT->Init();
	SetupEngineCamera();
	SetupGameCamera();
	SetupSkybox();
	SetupSunLight();

	// Create Cursor
	m_pCursor = PToA->MakeObject("../Resources/Prefab/Cursor.object.json");
	m_pCursor->Init();
	OBJECT->SetCursorActor(m_pCursor);

	UI->DoFadeOut();
}

void Game::Tick()
{
	STAGE->Tick();
	if (INPUT->GetButton(G))
	{
		EVENT->TriggerEvent(EventType::EVENT_LADDER, L"I_Ladder1");
		EVENT->TriggerEvent(EventType::EVENT_FENCE, L"Fence1");
	}

	UpdateCursor();

	if (ENGINE->m_bGamePaused == false &&
		INPUT->GetButton(GameKey::ESC))
		ENGINE->m_bGamePaused = !ENGINE->m_bGamePaused;

	UpdateUI();

	m_pSky->AddRotation(Vec3(0.0f, 0.05f * TIMER->GetDeltaTime(), 0.0f));

	//bgm
	{
		SOUND->GetPtr(ESoundType::Stage0)->Play2D();
	}
	//wind	
	{
		if (INPUT->GetButton(M))
		{
			m_bWind = !m_bWind;
		}

		if (m_bWind)
			CreateWind();

	}

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

	CheckFrustumCulling();
	CheckEnemyCollision();
	CheckBloodCollision();
	PROJECTILE->Tick();
}

void Game::Render()
{

}

void Game::Destroy()
{
	m_pCameraActor = nullptr;
	m_pGameCameraActor = nullptr;
	m_pPlayer = nullptr;
	m_pSkyMesh = nullptr;
	m_pSky = nullptr;

	m_vPausedBackGround.clear();
	m_vUpgradeBackGround.clear();
	m_vUpgradeState.clear();
	m_vCoins.clear();

	enemyList.clear();
	m_vObjectList.clear();
	m_vMapList.clear();
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
	m_pSky->m_bCastShadow = false;

	shared_ptr<UMaterial> material = make_shared<UMaterial>();
	material->Load(L"../Resources/Texture/skypano.png", L"../Resources/Shader/Sky.hlsl");
	m_pSkyMesh->SetMaterial(material);

	OBJECT->AddActor(m_pSky);
}

void Game::SetupSunLight()
{
	LIGHT->Clear();

	m_pSunLight = make_shared<ALight>();
	m_pSunLight->m_szName = L"SunLight";
	m_pSunLight->GetLightComponent()->SetDirection({ -1.f, -1.f, -1.f });
	m_pSunLight->GetLightComponent()->SetAmbientColor(Vec3(1.0f, 1.0f, 1.0f));
	m_pSunLight->GetLightComponent()->SetAmbientPower(0.3f);

	Vec3 dir = Vec3(-1.f, -1.f, -1.f);
	m_pSunLight->SetScale(Vec3(10.0f, 10.0f, 10.0f));
	m_pSunLight->SetPosition(dir * -300.f);
	dir.Normalize();
	m_pSunLight->GetCameraComponent()->SetLookTo(dir);
	m_pSunLight->m_bCastShadow = false;

	LIGHT->RegisterLight(m_pSunLight);
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
			float randomAngle = RandomRange(2.4f, 2.6f);
			wind->SetRotation(Vec3(0, 0, randomAngle));     // ↘ 방향
			float speed = RandomRange(0.35f, 0.45f);     // 바람 세기

			Vec3 velocity = Vec3(1, -1, 0) * speed;
			wind->SetVelocity(velocity);


			WIND->AddWind(wind);
		}

	}
}

void Game::CreateUI()
{
	// InGame UI
	UI->AddUIList(PToA->MakeUIs("../Resources/Prefab/UI_Game_BackGround.uis.json"));

	m_vHPUI = PToA->MakeUIs("../Resources/Prefab/UI_Game_HP.uis.json");
	m_vArrowUI = PToA->MakeUIs("../Resources/Prefab/UI_Game_Arrow.uis.json");
	m_vInterActionUI = PToA->MakeUIs("../Resources/Prefab/UI_InterAction.uis.json");
	UI->AddUIList(m_vHPUI);
	UI->AddUIList(m_vArrowUI);
	UI->AddUIList(m_vInterActionUI);

	for (auto& pUI : m_vInterActionUI)
	{
		pUI->m_bRender = false;
		pUI->m_bRun = false;
	}

	m_pActiveArrowTexture = TEXTURE->Get(L"Resources/Texture/UI/hud_energy_active.png");
	m_pInActiveArrowTexture = TEXTURE->Get(L"Resources/Texture/UI/hud_energy_inactive.png");

	m_vActiveArrowScale = m_vArrowUI[3]->GetScale();
	m_vInActiveArrowScale = m_vArrowUI[2]->GetScale();

	// Paused UI
	m_vPausedBackGround = PToA->MakeUIs("../Resources/Prefab/UI_Paused_BackGround.uis.json");
	UI->AddUIList(m_vPausedBackGround);
	for (auto& pUI : m_vPausedBackGround)
	{
		pUI->m_bRun = false;
		pUI->m_bRender = false;
	}

	m_vPausedSelect = PToA->MakeUIs("../Resources/Prefab/UI_Paused_Selection.uis.json");
	UI->AddUIList(m_vPausedSelect);
	for (auto& pUI : m_vPausedSelect)
	{
		pUI->m_bRun = false;
		pUI->m_bRender = false;
	}

	// Paused Upgrade
	m_vUpgradeBackGround = PToA->MakeUIs("../Resources/Prefab/UI_Paused_Upgrade_BackGround.uis.json");
	UI->AddUIList(m_vUpgradeBackGround);
	for (auto& pUI : m_vUpgradeBackGround)
	{
		pUI->m_bRun = false;
		pUI->m_bRender = false;
	}

	auto vUpgradeState = PToA->MakeUIs("../Resources/Prefab/UI_Paused_Upgrade_State.uis.json");
	UI->AddUIList(vUpgradeState);
	for (int iRow = 0; iRow < 4; iRow++)
	{
		vector<shared_ptr<AUIActor>> vUI;
		for (int iCol = 0; iCol < 8; iCol++)
		{
			vUpgradeState[iRow * 8 + iCol]->m_bRender = false;
			vUpgradeState[iRow * 8 + iCol]->m_bRun = false;
			vUI.emplace_back(vUpgradeState[iRow * 8 + iCol]);
		}
		m_vUpgradeState.emplace_back(vUI);
	}

	// Paused System
	m_vSystemBackGround = PToA->MakeUIs("../Resources/Prefab/UI_Paused_System_BackGround.uis.json");
	UI->AddUIList(m_vSystemBackGround);
	for (auto& pUI : m_vSystemBackGround)
	{
		pUI->m_bRun = false;
		pUI->m_bRender = false;
	}

	m_vSystemSelection = PToA->MakeUIs("../Resources/Prefab/UI_Paused_System_Selection.uis.json");
	UI->AddUIList(m_vSystemSelection);
	for (auto& pUI : m_vSystemSelection)
	{
		pUI->m_bRun = false;
		pUI->m_bRender = false;
	}

	m_vCoins = PToA->MakeUIs("../Resources/Prefab/UI_Game_Coins.uis.json");
	UI->AddUIList(m_vCoins);

	// Dead
	m_pDeadUI = PToA->MakeUI("../Resources/Prefab/UI_Dead.ui.json");
	UI->AddUI(m_pDeadUI);
}

void Game::UpdateUI()
{
	// HP
	Color RestColor;
	RestColor = fullHP;
	RestColor.w = -0.5f;

	static float currentTime = 0.0f;
	currentTime = TIMER->GetDeltaTime();

	auto currentHP = dynamic_pointer_cast<TCharacter>(m_pPlayer)->GetHp();

	if (currentHP != m_iPreHP)
		m_bHPUIChange = true;

	switch (currentHP)
	{
	case 4:
	{
		m_vHPUI[0]->SetColor(RestColor);
		m_vHPUI[1]->SetColor(RestColor);
		m_vHPUI[2]->SetColor(RestColor);
		m_vHPUI[3]->SetColor(fullHP);
	}
	break;

	case 3:
	{
		if (m_vHPUI[2]->GetColor().w < 0.f && m_bHPUIChange)
		{
			m_vHPUI[3]->SetColor(Color(0.f, 0.f, 0.f, -1.f));
			m_vHPUI[2]->AddColor(Color(0.f, 0.f, 0.f, currentTime / 2));
		}
		else if (m_bHPUIChange == true)
			m_bHPUIChange = false;
		else
		{
			m_vHPUI[0]->SetColor(RestColor);
			m_vHPUI[1]->SetColor(RestColor);
			m_vHPUI[2]->SetColor(fullHP);
		}
	}
	break;

	case 2:
	{
		if (m_vHPUI[1]->GetColor().w < 0.f && m_bHPUIChange)
		{
			m_vHPUI[2]->SetColor(Color(0.f, 0.f, 0.f, -1.f));
			m_vHPUI[1]->AddColor(Color(0.f, 0.f, 0.f, currentTime / 2));
		}
		else if (m_bHPUIChange == true)
			m_bHPUIChange = false;
		else
		{
			m_vHPUI[0]->SetColor(RestColor);
			m_vHPUI[1]->SetColor(fullHP);
		}
	}
	break;

	case 1:
	{
		if (m_vHPUI[0]->GetColor().w < 0.f && m_bHPUIChange)
		{
			m_vHPUI[1]->SetColor(Color(0.f, 0.f, 0.f, -1.f));
			m_vHPUI[0]->AddColor(Color(0.f, 0.f, 0.f, currentTime / 2));
		}
		else if (m_bHPUIChange == true)
			m_bHPUIChange = false;
		else
			m_vHPUI[0]->SetColor(fullHP);

	}
	break;

	case 0:
	{
		m_vHPUI[0]->SetColor(Color(0.f, 0.f, 0.f, -1.f));
	}
	break;
	}

	m_iPreHP = currentHP;

	// Arrow
	switch (dynamic_pointer_cast<TPlayer>(m_pPlayer)->GetArrowCount())
	{
	case 4:
		m_vArrowUI[0]->SetAllTexture(m_pInActiveArrowTexture);
		m_vArrowUI[1]->SetAllTexture(m_pInActiveArrowTexture);
		m_vArrowUI[2]->SetAllTexture(m_pInActiveArrowTexture);
		m_vArrowUI[3]->SetAllTexture(m_pActiveArrowTexture);

		m_vArrowUI[0]->SetScale(m_vInActiveArrowScale);
		m_vArrowUI[1]->SetScale(m_vInActiveArrowScale);
		m_vArrowUI[2]->SetScale(m_vInActiveArrowScale);
		m_vArrowUI[3]->SetScale(m_vActiveArrowScale);

		m_vArrowUI[0]->m_bRender = true;
		m_vArrowUI[1]->m_bRender = true;
		m_vArrowUI[2]->m_bRender = true;
		m_vArrowUI[3]->m_bRender = true;
		break;

	case 3:
		m_vArrowUI[0]->SetAllTexture(m_pInActiveArrowTexture);
		m_vArrowUI[1]->SetAllTexture(m_pInActiveArrowTexture);
		m_vArrowUI[2]->SetAllTexture(m_pActiveArrowTexture);

		m_vArrowUI[0]->SetScale(m_vInActiveArrowScale);
		m_vArrowUI[1]->SetScale(m_vInActiveArrowScale);
		m_vArrowUI[2]->SetScale(m_vActiveArrowScale);

		m_vArrowUI[0]->m_bRender = true;
		m_vArrowUI[1]->m_bRender = true;
		m_vArrowUI[2]->m_bRender = true;
		m_vArrowUI[3]->m_bRender = false;
		break;

	case 2:
		m_vArrowUI[0]->SetAllTexture(m_pInActiveArrowTexture);
		m_vArrowUI[1]->SetAllTexture(m_pActiveArrowTexture);

		m_vArrowUI[0]->SetScale(m_vInActiveArrowScale);
		m_vArrowUI[1]->SetScale(m_vActiveArrowScale);

		m_vArrowUI[0]->m_bRender = true;
		m_vArrowUI[1]->m_bRender = true;
		m_vArrowUI[2]->m_bRender = false;
		m_vArrowUI[3]->m_bRender = false;
		break;

	case 1:
		m_vArrowUI[0]->SetAllTexture(m_pActiveArrowTexture);

		m_vArrowUI[0]->SetScale(m_vActiveArrowScale);

		m_vArrowUI[0]->m_bRender = true;
		m_vArrowUI[1]->m_bRender = false;
		m_vArrowUI[2]->m_bRender = false;
		m_vArrowUI[3]->m_bRender = false;
		break;

	case 0:
		m_vArrowUI[0]->m_bRender = false;
		m_vArrowUI[1]->m_bRender = false;
		m_vArrowUI[2]->m_bRender = false;
		m_vArrowUI[3]->m_bRender = false;
		break;
	}

	// InterAction, 상호작용 UI를 띄우는 구간
	auto vTriggerList = dynamic_pointer_cast<TPlayer>(m_pPlayer)->GetTrigger();
	if (vTriggerList.eTriggerType != ETriggerType::TT_NONE)
	{
		static Vec3 offsetPos1 = m_vInterActionUI[1]->GetPosition();
		static Vec3 offsetPos2 = m_vInterActionUI[2]->GetPosition();

		auto pos = CAMERA->GetScreenPos(vTriggerList.vPoint);

		m_vInterActionUI[0]->SetPosition(pos);
		m_vInterActionUI[1]->SetPosition(pos + offsetPos1);
		m_vInterActionUI[2]->SetPosition(pos + offsetPos2);

		for (auto& pUI : m_vInterActionUI)
		{
			pUI->m_bRun = true;
			pUI->m_bRender = true;

			if (vTriggerList.eTriggerType == ETriggerType::TT_LADDER)
			{

			}
			else if (vTriggerList.eTriggerType == ETriggerType::TT_HEALPOINT)
			{

			}
		}		
	}
	else
	{
		for (auto& pUI : m_vInterActionUI)
		{
			pUI->m_bRun = false;
			pUI->m_bRender = false;
		}
	}

	// Paused
	if (ENGINE->m_bGamePaused == true)
	{
		for (auto& pUI : m_vPausedBackGround)
		{
			pUI->m_bRun = true;
			pUI->m_bRender = true;
		}

		for(auto& pUI : m_vPausedSelect)
		{
			pUI->m_bRun = true;
			pUI->m_bRender = true;
		}

		// 0이 없그레이드, 1이 설정
		if (m_vPausedSelect[0]->GetStateType() == UIStateType::ST_SELECT)
			m_iSelectUI = 0;
		else if(m_vPausedSelect[1]->GetStateType() == UIStateType::ST_SELECT)
			m_iSelectUI = 1;
		
		switch (m_iSelectUI)
		{
			// Upgrade
		case 0:
		{
			// 선택된 옵션 밝게
			m_vPausedSelect[0]->SetColor(Color(0.f, 0.f, 0.f, 0.f));
			m_vPausedSelect[1]->SetColor(Color(0.f, 0.f, 0.f, -0.3f));

			// 꺼줄 UI
			for (auto& pUI : m_vSystemBackGround)
			{
				pUI->m_bRun = false;
				pUI->m_bRender = false;
			}

			for (auto& pUI : m_vSystemSelection)
			{
				pUI->m_bRun = false;
				pUI->m_bRender = false;
			}

			// 설정값
			for (auto& pUI : m_vUpgradeBackGround)
			{
				pUI->m_bRun = true;
				pUI->m_bRender = true;
			}

			UINT iSelect = 1;
			for (auto& pUIList : m_vUpgradeState)
			{
				// 해당 Upgrade 부분을 선택하면 색깔이 바뀌게 하는 로직
				if (pUIList[0]->GetStateType() == UIStateType::ST_SELECT)
				{
					m_iSelectUpgradeUI = iSelect;
				}

				if (iSelect == m_iSelectUpgradeUI)
					pUIList[1]->SetColor(Color(0.f, 0.f, 0.f, 0.0f));
				else
					pUIList[1]->SetColor(Color(0.f, 0.f, 0.f, -0.3f));

				for (auto& pUI : pUIList)
				{
					pUI->m_bRun = true;
					pUI->m_bRender = true;
				}

				iSelect++;
			}

			// 선택된 Upgrade의 설명이 적힌 부분을 Render한다.
			switch (m_iSelectUpgradeUI)
			{
			case 1: 
				break;

			case 2:
				break;

			case 3:
				break;

			case 4:
				break;
			default:
				break;
			}

			break;
		}
		// System
		case 1:
		{
			// 선택된 옵션 밝게
			m_vPausedSelect[0]->SetColor(Color(0.f, 0.f, 0.f, -0.3f));
			m_vPausedSelect[1]->SetColor(Color(0.f, 0.f, 0.f, 0.f));

			// 꺼줄 UI
			for (auto& pUI : m_vUpgradeBackGround)
			{
				pUI->m_bRun = false;
				pUI->m_bRender = false;
			}

			for (auto& pUIList : m_vUpgradeState)
				for (auto& pUI : pUIList)
				{
					pUI->m_bRun = false;
					pUI->m_bRender = false;
				}

			m_iSelectUpgradeUI = 0;

			// 설정값
			for (auto& pUI : m_vSystemBackGround)
			{
				pUI->m_bRun = true;
				pUI->m_bRender = true;
			}

			for (auto& pUI : m_vSystemSelection)
			{
				pUI->m_bRun = true;
				pUI->m_bRender = true;
			}

			if (m_vSystemSelection[0]->GetStateType() == UIStateType::ST_SELECT)
				ENGINE->m_bGamePaused = false;

			if (m_vSystemSelection[1]->GetStateType() == UIStateType::ST_SELECT)
				ENGINE->m_bRun = false;

			break;
		}
		}

		OBJECT->SetCursorActor(nullptr);
	}
	// Paused 가 아닐 때
	else
	{
		for (auto& pUI : m_vPausedBackGround)
		{
			pUI->m_bRun = false;
			pUI->m_bRender = false;
		}

		for (auto& pUI : m_vPausedSelect)
		{
			pUI->m_bRun = false;
			pUI->m_bRender = false;
		}

		for (auto& pUI : m_vUpgradeBackGround)
		{
			pUI->m_bRun = false;
			pUI->m_bRender = false;
		}

		for (auto& pUIList : m_vUpgradeState)
			for (auto& pUI : pUIList)
			{
				pUI->m_bRun = false;
				pUI->m_bRender = false;
			}

		for (auto& pUI : m_vSystemBackGround)
		{
			pUI->m_bRun = false;
			pUI->m_bRender = false;
		}

		for (auto& pUI : m_vSystemSelection)
		{
			pUI->m_bRun = false;
			pUI->m_bRender = false;
		}

		m_iSelectUpgradeUI = 0;

		OBJECT->SetCursorActor(m_pCursor);
	}

	// End
	static float tempTime = 0;
	if (dynamic_pointer_cast<TPlayer>(m_pPlayer)->IsDead())
	{
		tempTime += TIMER->GetDeltaTime();

		if (tempTime > m_fDeadUIPopTime)
		{
			m_pDeadUI->m_bRender = true;
			m_vCoins[1]->m_bRender = false;
			m_vCoins[3]->m_bRender = false;
		}
	}
	else
	{
		m_pDeadUI->m_bRender = false;
		tempTime = 0.f;
		m_vCoins[1]->m_bRender = true;
		m_vCoins[3]->m_bRender = true;
	}
}

void Game::UpdateCursor()
{
	// 위치
	// 마우스 Ray로 Character와 동일한 z값을 가진 평면의 Intersection을 구함
	MouseRay ray;
	ray.Click();

	auto playerPos = m_pPlayer->GetPosition();
	playerPos.y += 1.f;
	Vec3 inter;

	Collision::GetIntersection(ray, playerPos, Vec3(0.f, 1.f, 0.f), inter);

	auto rotDir = inter - playerPos;
	auto rotVec = rotDir;
	rotVec.Normalize();

	// 특정 거리를 넘어가지 않도록 반지름 지정
	float radius = 7.5f;
	if (rotDir.Length() > radius)
		inter = playerPos + rotVec * radius;

	m_pCursor->SetPosition(inter);

	// 회전
	// 캐릭터 -> 마우스 벡터와 Vec3(1, 0, 0)을 내적하여 각을 구한다.
	float angle;
	float fDot = rotVec.Dot(Vec3(1.f, 0.f, 0.f));

	if (rotVec.z > 0.f)
		angle = -(acosf(fDot) - DD_PI / 2.f);
	else
		angle = (acosf(fDot) + DD_PI / 2.f);

	m_pCursor->SetRotation(Vec3(DD_PI / 2.f, angle, 0.f));
}

void Game::SetEnemy(vector<shared_ptr<AActor>>& _enemyList)
{
	for (auto& enemy : _enemyList)
	{
		auto e = dynamic_pointer_cast<TEnemy>(enemy);
		if (e)
		{
			e->SetPlayer(m_pPlayer);
		}
	}
}

void Game::CheckFrustumCulling()
{
	auto vFrustum = CAMERA->GetFrustum();

	for (auto& pEnemy : enemyList)
	{
		if (Collision::CheckOBBToPlane(dynamic_pointer_cast<UBoxComponent>(pEnemy->GetShapeComponent())->GetBounds(), vFrustum[0]) < 0 ||
			Collision::CheckOBBToPlane(dynamic_pointer_cast<UBoxComponent>(pEnemy->GetShapeComponent())->GetBounds(), vFrustum[1]) < 0 ||
			Collision::CheckOBBToPlane(dynamic_pointer_cast<UBoxComponent>(pEnemy->GetShapeComponent())->GetBounds(), vFrustum[2]) < 0 ||
			Collision::CheckOBBToPlane(dynamic_pointer_cast<UBoxComponent>(pEnemy->GetShapeComponent())->GetBounds(), vFrustum[3]) < 0 ||
			Collision::CheckOBBToPlane(dynamic_pointer_cast<UBoxComponent>(pEnemy->GetShapeComponent())->GetBounds(), vFrustum[4]) < 0 ||
			Collision::CheckOBBToPlane(dynamic_pointer_cast<UBoxComponent>(pEnemy->GetShapeComponent())->GetBounds(), vFrustum[5]) < 0)
			continue;

		dynamic_pointer_cast<TEnemy>(pEnemy)->SetFrustumIn(true);
	}
}

void Game::CheckEnemyCollision()
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
		COLLITION->CheckCollision(m_pPlayer, *iter);
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

	// 이걸 또 영역 별로 넣어야하네 
	if (melee)
	{
		for (auto iter = enemyList.begin(); iter != enemyList.end();)
		{
			if (dynamic_cast<TEnemy*>(iter->get())->IsFrustumIn() == false)
			{
				iter++;
				continue;
			}
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
		if (dynamic_cast<TEnemy*>(iter->get())->IsFrustumIn() == false)
		{
			iter++;
			continue;
		}
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
		//COLLITION->CheckCollision(tempHeadRoller, *iter);
		COLLITION->CheckCollision(m_pBetty, *iter);
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

	auto list = PROJECTILE->GetActorList();
	for (auto proj = list.begin(); proj != list.end(); )
	{
		COLLITION->CheckCollision(*proj, m_pPlayer);

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
			if (dynamic_cast<TEnemy*>(iter->get())->IsFrustumIn() == false)
			{
				iter++;
				continue;
			}
			if ((iter->get() == nullptr) || iter->get()->m_bDelete == true)
			{
				iter = enemyList.erase(iter);
				continue;
			}
			COLLITION->CheckCollision(*proj, *iter);
			iter++;
		}

		if (melee)
		{
			COLLITION->CheckCollision(*proj, melee);
		}
		proj++;
	}
}

void Game::CheckBloodCollision()
{
	auto& bloodList = EFFECT->GetBloodList();
	for (auto blood = bloodList.begin(); blood != bloodList.end(); )
	{
		if (blood->get()->GetPosition().y <= 0.f)
			int i = 0;

		if (!(*blood)->IsActive() || (*blood)->m_bDelete)
		{
			blood = bloodList.erase(blood);
			continue;
		}

		for (auto iter = m_vMapList.begin(); iter != m_vMapList.end();)
		{
			if ((iter->get() == nullptr) || iter->get()->m_bDelete == true)
			{
				iter = m_vMapList.erase(iter);
				continue;
			}
			COLLITION->CheckCollision(*blood, *iter);
			iter++;
		}

		blood++;
	}
}

