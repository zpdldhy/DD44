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

// Script
#include "ScriptManager.h"
#include "EngineCameraMoveScript.h"
#include "GameCameraMove.h"
#include "MageMovement.h"

// Game
#include "ProjectileManager.h"
#include "EnemyCollisionManager.h"
#include "EventManager.h"

// TEMP
#include "BatMovement.h"
#include "WalkerMovement.h"
#include "BettyMovement.h"
#include "Timer.h"

void Game::Init()
{
	SCRIPT->Init();
	// Asset 로딩
	PToA->Init();
	m_vMapList = PToA->LoadAllPrefabs(".map.json");
	for (auto map : m_vMapList)
	{
		map->m_bCastShadow = false;
	}
	m_vObjectList = PToA->LoadAllPrefabs(".objects.json");
	OBJECT->AddActorList(m_vMapList);
	OBJECT->AddActorList(m_vObjectList);
	OBJECT->AddActorList(PToA->LoadAllPrefabs(".particlegroup.json"));

	m_pPlayer = PToA->MakeCharacter("../Resources/Prefab/Player/Mycharacter.character.json");
	m_pPlayer->SetUseStencil(true);
	OBJECT->AddActor(m_pPlayer);

	auto objectList = PToA->LoadAllPrefabs(".character.json");
	OBJECT->AddActorList(objectList);
	for (auto interactable : objectList)
	{
		m_vObjectList.push_back(interactable);
	}

	m_pBetty = PToA->MakeCharacter("../Resources/Prefab/Player/Boss_Betty_test.character.json");
	
	enemyList1 = PToA->LoadAllPrefabs(".character.json", "../Resources/Prefab/Stage01/");
	enemyList1.emplace_back(m_pBetty);
	SetEnemy(enemyList1);
	OBJECT->AddActorList(enemyList1);
	
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
}

void Game::Tick()
{
	if (INPUT->GetButton(G))
	{
		EVENT->TriggerEvent(EventType::EVENT_LADDER, L"I_Ladder");
		EVENT->TriggerEvent(EventType::EVENT_FENCE, L"Fence1");
	}

	UpdateCursor();

	if (INPUT->GetButton(GameKey::ESC))
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

	CheckEnemyCollision();
	CheckBloodCollision();
	PROJECTILE->Tick();
	CheckEnemyDeath(enemyList1);
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

	enemyList1.clear();
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
	UI->AddUIList(m_vHPUI);
	UI->AddUIList(m_vArrowUI);

	m_pActiveArrowTexture = TEXTURE->Get(L"Resources/Texture/UI/hud_energy_active.png");
	m_pInActiveArrowTexture = TEXTURE->Get(L"Resources/Texture/UI/hud_energy_inactive.png");

	m_vActiveArrowScale = m_vArrowUI[3]->GetScale();
	m_vInActiveArrowScale = m_vArrowUI[2]->GetScale();

	// Paused UI
	m_vPausedBackGround = PToA->MakeUIs("../Resources/Prefab/UI_Paused_BackGround.uis.json");
	UI->AddUIList(m_vPausedBackGround);
	m_vUpgradeBackGround = PToA->MakeUIs("../Resources/Prefab/UI_Paused_Upgrade_BackGround.uis.json");
	UI->AddUIList(m_vUpgradeBackGround);
	m_vUpgradeState = PToA->MakeUIs("../Resources/Prefab/UI_Paused_Upgrade_State.uis.json");
	UI->AddUIList(m_vUpgradeState);
	m_vCoins = PToA->MakeUIs("../Resources/Prefab/UI_Game_Coins.uis.json");
	UI->AddUIList(m_vCoins);

	// Dead
	m_pDeadUI = PToA->MakeUI("../Resources/Prefab/UI_Dead.ui.json");
	UI->AddUI(m_pDeadUI);

	UI->DoFadeOut();
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

	if(currentHP!= m_iPreHP)
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

	// Paused
	if (ENGINE->m_bGamePaused == true)
	{
		for (auto& pUI : m_vPausedBackGround)
			pUI->m_bRender = true;

		for (auto& pUI : m_vUpgradeBackGround)
			pUI->m_bRender = true;

		for (auto& pUI : m_vUpgradeState)
			pUI->m_bRender = true;

		OBJECT->SetCursorActor(nullptr);
	}
	else
	{
		for (auto& pUI : m_vPausedBackGround)
			pUI->m_bRender = false;

		for (auto& pUI : m_vUpgradeBackGround)
			pUI->m_bRender = false;

		for (auto& pUI : m_vUpgradeState)
			pUI->m_bRender = false;

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
		for (auto iter = enemyList1.begin(); iter != enemyList1.end();)
		{
			if ((iter->get() == nullptr) || iter->get()->m_bDelete == true)
			{
				iter = enemyList1.erase(iter);
				continue;
			}
			
			if (COLLITION->CheckCollision(*iter, melee))
			{
				auto enemy = dynamic_pointer_cast<TEnemy>(*iter);
				if (enemy)
				{
					float damage = dynamic_pointer_cast<TPlayer>(m_pPlayer)->GetMeleeDamage();
					enemy->SetDamaged(damage);
				}
			}

			iter++;
		}
	}

	for (auto iter = enemyList1.begin(); iter != enemyList1.end();)
	{
		if ((iter->get() == nullptr) || iter->get()->m_bDelete == true)
		{
			iter = enemyList1.erase(iter);
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
		//COLLITION->CheckCollision(m_pBetty, *iter);
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

		for (auto iter = enemyList1.begin(); iter != enemyList1.end();)
		{
			if ((iter->get() == nullptr) || iter->get()->m_bDelete == true)
			{
				iter = enemyList1.erase(iter);
				continue;
			}
			COLLITION->CheckCollision(*proj, *iter);
			iter++;
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

void Game::CheckEnemyDeath(const vector<shared_ptr<AActor>>& _enemyList)
{
	for (const auto& actor : _enemyList)
	{
		if (!actor) continue;

		TCharacter* enemy = dynamic_cast<TCharacter*>(actor.get());
		if (enemy && enemy->IsDead())
		{
			if (auto player = std::dynamic_pointer_cast<TPlayer>(m_pPlayer))
			{
				player->AddSoul(enemy->GetSoul());
			}
		}
	}
}

