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
	//m_vObjectList = PToA->LoadAllPrefabs(".objects.json");

	stage0 = PToA->MakeObjects("../Resources/Prefab/Stage_0.objects.json");
	stage1 = PToA->MakeObjects("../Resources/Prefab/Stage_1.objects.json");
	stage2 = PToA->MakeObjects("../Resources/Prefab/Stage_2.objects.json");

	for (auto obj : stage0)
	{
		m_vObjectList.emplace_back(obj);
	}
	for (auto obj : stage1)
	{
		m_vObjectList.emplace_back(obj);
	}
	for (auto obj : stage2)
	{
		m_vObjectList.emplace_back(obj);
	}

	OBJECT->AddActorList(m_vMapList);
	OBJECT->AddActorList(m_vObjectList);
	OBJECT->AddActorList(PToA->LoadAllPrefabs(".particlegroup.json"));

	m_pPlayer = PToA->MakeCharacter("../Resources/Prefab/Player/Mycharacter.character.json");
	m_pPlayer->SetUseStencil(true);
	//m_pPlayer->SetPosition(Vec3(-90, 39, 70));
	//m_pPlayer->SetPosition(Vec3(10, 0, 0));
	//m_pPlayer->SetPosition(Vec3(-63, 28, 26));
	//m_pPlayer->SetPosition(Vec3(78.6, -0.32, -100));
	OBJECT->AddActor(m_pPlayer);

	auto objectList = PToA->LoadAllPrefabs(".character.json");
	OBJECT->AddActorList(objectList);

	for (auto& interactable : objectList)
	{
		m_vObjectList.push_back(interactable);
		stage0.push_back(interactable);
		stage1.push_back(interactable);
		stage2.push_back(interactable); 

	}

	auto fenceList = ENEMYCOLLIDER->triggerObjectList;
	for (auto& obj : fenceList)
	{
		m_vObjectList.push_back(obj);
		stage0.push_back(obj);
		stage1.push_back(obj);
		stage2.push_back(obj);
	}

	m_pBetty = PToA->MakeCharacter("../Resources/Prefab/Player/Boss_Betty.character.json");
	enemyList.emplace_back(m_pBetty);

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
		collisionEnemy1.emplace_back(e);
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

	// UI
	m_cUI.init();

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

	m_cUI.Destroy();
	m_pCursor = nullptr;

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
	m_pSunLight->GetLightComponent()->SetAmbientPower(.7f);

	Vec3 dir = Vec3(-.7f, -1.f, -.7f);
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

void Game::UpdateUI()
{
	static int i = 4;
	static int coin = 0;

	if (INPUT->GetButton(GameKey::P))
		i++;		

	if (INPUT->GetButtonDown(GameKey::I))
		coin += 100;

	// UI가 적용해야 하는 부분
	m_cUI.SetMaxHP(i);
	m_cUI.SetCurrentHP(dynamic_pointer_cast<TPlayer>(m_pPlayer)->GetHp());
	m_cUI.SetArrowCount(dynamic_pointer_cast<TPlayer>(m_pPlayer)->GetArrowCount());
	m_cUI.SetTriggerData(dynamic_pointer_cast<TPlayer>(m_pPlayer)->GetTrigger());	
	m_cUI.SetCoin(coin);
	m_cUI.SetDead(dynamic_pointer_cast<TPlayer>(m_pPlayer)->IsDead());

	// 가격 출력하는 부분
	m_cUI.SetHealthPrice(200);

	// 업그레이드 실패시
	if (INPUT->GetButtonDown(GameKey::U))
		m_cUI.IsBuyUpgrade(false);
	else
		m_cUI.IsBuyUpgrade(true);

	m_cUI.Tick();

	// UI의 상태에 따라 적용되는 부분
	///////////////////////////////////////////////////////////////////////////
	///////////						Paused							///////////
	///////////////////////////////////////////////////////////////////////////

	// Upgrade State
	// 0은 Default로 Upgrade 없음을 나타냄
	switch (m_cUI.SelectUpgrade())
	{
	case 1: // MaxHP
		// MaxHP++
		break;

	case 2: // Attack
		break;

	case 3: // Speed
		break;

	case 4:	// Arrow
		break;
	}

	// 계속하기
	if(m_cUI.SelectContinue())
		ENGINE->m_bGamePaused = false;

	// 게임 종료
	if(m_cUI.SelectExit())
		PostQuitMessage(0);

	///////////////////////////////////////////////////////////////////////////
	///////////						Dead							///////////
	///////////////////////////////////////////////////////////////////////////

	// Continue UI 선택시, 사운드 등
	if (m_cUI.SelectDeadContinue())
	{

	}

	// Dead UI 종료 시, 캐릭 살아남 등
	if (m_cUI.EndDeadUI())
	{
		dynamic_pointer_cast<TPlayer>(m_pPlayer)->SetHp(4);
		// Dead true 세팅
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

	if(ENGINE->m_bGamePaused == true)
		OBJECT->SetCursorActor(nullptr);
	else
		OBJECT->SetCursorActor(m_pCursor);
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
	StagePhase currentStage = STAGE->GetCurrentStage();
	auto projectileList = PROJECTILE->GetActorList();
	switch (currentStage)
	{
	case StagePhase::STAGE00:
		for (auto proj = projectileList.begin(); proj != projectileList.end(); )
		{
			COLLITION->CheckCollision(*proj, m_pPlayer);

			for (auto iter = stage0.begin(); iter != stage0.end();)
			{
				if ((iter->get() == nullptr) || iter->get()->m_bDelete == true)
				{
					iter = stage0.erase(iter);
					continue;
				}
				COLLITION->CheckCollision(*proj, *iter);
				iter++;
			}
			proj++;
		}
	break;
	case StagePhase::STAGE01:
		for (auto proj = projectileList.begin(); proj != projectileList.end(); )
		{
			COLLITION->CheckCollision(*proj, m_pPlayer);

			for (auto iter = stage0.begin(); iter != stage0.end();)
			{
				if ((iter->get() == nullptr) || iter->get()->m_bDelete == true)
				{
					iter = stage0.erase(iter);
					continue;
				}
				COLLITION->CheckCollision(*proj, *iter);
				iter++;
			}
			proj++;
		}
	break;
	case StagePhase::STAGE10:
		for (auto proj = projectileList.begin(); proj != projectileList.end(); )
		{
			COLLITION->CheckCollision(*proj, m_pPlayer);

			for (auto iter = stage1.begin(); iter != stage1.end();)
			{
				if ((iter->get() == nullptr) || iter->get()->m_bDelete == true)
				{
					iter = stage1.erase(iter);
					continue;
				}
				COLLITION->CheckCollision(*proj, *iter);
				iter++;
			}
			proj++;
		}

		for (auto enemy1 = collisionEnemy1.begin(); enemy1 != collisionEnemy1.end(); )
		{
			if ((enemy1->get() == nullptr) || enemy1->get()->m_bDelete == true)
			{
				enemy1 = collisionEnemy1.erase(enemy1);
				continue;
			}

			for (auto obj = stage1.begin(); obj != stage1.end();)
			{
				if ((obj->get() == nullptr) || obj->get()->m_bDelete == true)
				{
					obj = stage1.erase(obj);
					continue;
				}
				COLLITION->CheckCollision(*enemy1, *obj);
				obj++;
			}

			for (auto it2 = std::next(enemy1); it2 != collisionEnemy1.end(); ++it2)
			{
				if ((*it2 == nullptr) || (*it2)->m_bDelete)
					continue;

				COLLITION->CheckCollision(*enemy1, *it2);
			}

			enemy1++;
		}
		break;	
	case StagePhase::STAGE20:
		for (auto proj = projectileList.begin(); proj != projectileList.end(); )
		{
			COLLITION->CheckCollision(*proj, m_pPlayer);
			COLLITION->CheckCollision(*proj, m_pBetty);

			for (auto iter = stage2.begin(); iter != stage2.end();)
			{
				if ((iter->get() == nullptr) || iter->get()->m_bDelete == true)
				{
					iter = stage2.erase(iter);
					continue;
				}
				COLLITION->CheckCollision(*proj, *iter);
				iter++;
			}
			proj++;
		}
		for (auto obj = stage2.begin(); obj != stage2.end();)
		{
			if ((obj->get() == nullptr) || obj->get()->m_bDelete == true)
			{
				obj = stage2.erase(obj);
				continue;
			}
			COLLITION->CheckCollision(m_pBetty, *obj);
			obj++;
		}		
		break;
	}

	shared_ptr<AActor> melee;
	// 몬스터 공격 범위랑 플레이어 확인
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
		iter++;
	}
	// player melee랑 enemy 확인
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

	for (auto proj = projectileList.begin(); proj != projectileList.end(); )
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

