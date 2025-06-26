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
#include "ASoulActor.h"
#include "ABloodActor.h"

// Component
#include "UStaticMeshComponent.h"
#include "UBoxComponent.h"

// Script
#include "ScriptManager.h"
#include "EngineCameraMoveScript.h"
#include "GameCameraMove.h"
#include "MageMovement.h"
#include "PlayerMoveScript.h"

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

	// FadeOut 중엔 Paused 못하게
	//m_bNoPaused = true;
	//dynamic_pointer_cast<PlayerMoveScript>(m_pPlayer->GetScriptList()[0])->NoInput();
	UI->DoFadeOut();

	m_cBettyMovie.SetCameraStart(Vec3(64.f, 4.f, -94.f), Vec3(-0.06f, 11.65f, 0.f));

	//bgm
	{
		SOUND->GetPtr(ESoundType::Stage0)->Play2D();
	}
}

void Game::Tick()
{
	// FadeOut이 처음 완료 되었을 때,
	if (UI->GetFadeOutDone())
	{
		m_bNoPaused = false;
		dynamic_pointer_cast<PlayerMoveScript>(m_pPlayer->GetScriptList()[0])->CanInput();
	}

	// Betty가 죽으면
	if (dynamic_pointer_cast<BettyMovement>(m_pBetty->GetScriptList()[0])->IsBettyDie())
		//if (INPUT->GetButton(GameKey::P))
	{
		static bool isEnd = false;
		if (isEnd == false)
		{
		SOUND->GetPtr(ESoundType::Boss1)->Stop();
		SOUND->GetPtr(ESoundType::Ending)->Play2D();
		dynamic_pointer_cast<PlayerMoveScript>(m_pPlayer->GetScriptList()[0])->NoInput();
		dynamic_pointer_cast<PlayerMoveScript>(m_pPlayer->GetScriptList()[0])->EndGame();
		UI->DoFadeIn();
		m_cUI.NoRenderStateUI();
		m_cUI.GoEnding();
		}
		isEnd = true;
	}

	STAGE->Tick();
	if (STAGE->GetCurrentStage() == StagePhase::FINAL)
	{
		m_bStartBettyMoveScene = true;
	}

	UpdateCursor();

	if (ENGINE->m_bGamePaused == false &&
		INPUT->GetButton(GameKey::ESC) &&
		!m_bNoPaused)
		ENGINE->m_bGamePaused = !ENGINE->m_bGamePaused;

	UpdateUI();
	BettyMeetMovie();

	m_pSky->AddRotation(Vec3(0.0f, 0.05f * TIMER->GetDeltaTime(), 0.0f));


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
	CheckEnemyDeath(enemyList);

	if (m_pPlayer->GetPosition().y < -4.0f)
	{
		dynamic_pointer_cast<TPlayer>(m_pPlayer)->TakeDamage(1);
		m_pPlayer->SetPosition(Vec3(50.0f, 40.0f, 100.0f));
	}
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
			float startX = RandomRange(-1.2f, 0.f); // 살짝 바깥쪽에서 시작
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
	auto player = std::dynamic_pointer_cast<TPlayer>(m_pPlayer);

	m_cUI.SetMaxHP(player->GetMaxHP());
	m_cUI.SetCurrentHP(player->GetHp());

	m_cUI.SetMaxArrow(player->GetArrowCapacity());
	m_cUI.SetCurrentArrow(player->GetArrowCount());

	m_cUI.SetTriggerData(player->GetTrigger());

	m_cUI.SetCoin(player->GetHisSoul());
	m_cUI.SetDead(player->IsDead());

	// 가격 출력하는 부분
	m_cUI.SetHealthPrice(player->GetNextUpgradeCost(EStatType::HP));
	m_cUI.SetAttackPrice(player->GetNextUpgradeCost(EStatType::MeleeDamage));
	m_cUI.SetSpeedPrice(player->GetNextUpgradeCost(EStatType::MoveSpeed));
	m_cUI.SetArrowPrice(player->GetNextUpgradeCost(EStatType::RangedDamage));

	if (INPUT->GetButton(GameKey::SPACE))
	{
		switch (m_cUI.CurrentUpgrade())
		{
		case 1:
			m_cUI.IsBuyUpgrade(player->UpgradeStat(EStatType::HP));
			break;
		case 2:
			m_cUI.IsBuyUpgrade(player->UpgradeStat(EStatType::MeleeDamage));

			break;
		case 3:
			m_cUI.IsBuyUpgrade(player->UpgradeStat(EStatType::MoveSpeed));
			break;
		case 4:
			m_cUI.IsBuyUpgrade(player->UpgradeStat(EStatType::RangedDamage));
			break;
		}
	}
	else
		m_cUI.IsBuyUpgrade(true);

	m_cUI.Tick();

	// UI의 상태에 따라 적용되는 부분

	if (m_cUI.IsHealAction())
	{
		player->SetHp(player->GetMaxHP());

		// 사운드
		SOUND->GetPtr(ESoundType::Healing)->PlayEffect2D();
	}

	///////////////////////////////////////////////////////////////////////////
	///////////						Paused							///////////
	///////////////////////////////////////////////////////////////////////////

	// 계속하기
	if (m_cUI.SelectContinue())
		ENGINE->m_bGamePaused = false;

	// 게임 종료
	if (m_cUI.SelectExit())
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
		dynamic_pointer_cast<PlayerMoveScript>(m_pPlayer->GetScriptList()[0])->Resurrection();
		SOUND->GetPtr(ESoundType::Stage0)->Play2D();
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
	float radius = 15.f;
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

	if (ENGINE->m_bGamePaused == true)
		OBJECT->SetCursorActor(nullptr);
	else
		OBJECT->SetCursorActor(m_pCursor);
}

void Game::BettyMeetMovie()
{
	if (!m_bStartBettyMoveScene || m_cBettyMovie.IsEnd()) return;

	// 시작하기 전에 세팅할 것들
	if (!m_cBettyMovie.IsStart())
	{
		// 연출 위치 조작
		dynamic_pointer_cast<PlayerMoveScript>(m_pPlayer->GetScriptList()[0])->NoInput();
		m_pPlayer->GetPhysicsComponent()->SetWeight(0.f);
		auto pos = m_pPlayer->GetPosition();
		pos.y = 0.025f;

		m_cBettyMovie.SetPlayerEnterPos(pos);
		m_cBettyMovie.SetPlayerStartPos(Vec3(64.f, pos.y, -94.f));
		m_cBettyMovie.SetPlayerEndPos(Vec3(52.f, pos.y, -85.f));

		m_cBettyMovie.SetMoveTime1(3.f);
		m_cBettyMovie.SetRotTime1(4.f);
		m_cBettyMovie.SetTimeTrack1(7.f);

		m_cBettyMovie.SetMoveTime2(9.f);
		m_cBettyMovie.SetRestTime(11.f);
		m_cBettyMovie.SetTimeTrack2(12.f);

		m_cBettyMovie.SetUIPopUpTime(18.f);
		m_cBettyMovie.SetTimeTrack3(23.f);	// 베티 애니메이션 6초

		m_cUI.NoRenderStateUI();

		m_bNoPaused = true;

		SOUND->GetPtr(ESoundType::Stage0)->Stop();
	}

	m_cBettyMovie.StartMovie();
	m_cBettyMovie.Tick();

	if (m_cBettyMovie.StartTrack1() || m_cBettyMovie.StartTrack2())
	{
		m_pCursor->m_bRender = false;
		dynamic_pointer_cast<PlayerMoveScript>(m_pPlayer->GetScriptList()[0])->WalkAnim();
	}

	if (m_cBettyMovie.EndMove())
	{
		dynamic_pointer_cast<PlayerMoveScript>(m_pPlayer->GetScriptList()[0])->IdleAnim();
	}

	if (m_cBettyMovie.StartTrack3())
	{
		// 베티가 화내기 시작하는 부분
		EVENT->TriggerEvent(EventType::EVENT_STAGE, L"StartIntro");
	}

	if (m_cBettyMovie.IsUIPopUp())
	{
		m_cUI.PopUpBettyName();
	}

	// 장면이 완전 종료되었을 때
	if (m_cBettyMovie.IsEnd())
	{
		SOUND->GetPtr(ESoundType::Boss1)->Play2D();
		// 베티 움직이기 시작하는 구간
		EVENT->TriggerEvent(EventType::EVENT_STAGE, L"FinishIntro");

		m_cUI.PopDownBettyName();
		m_cUI.RenderStateUI();
		CAMERA->Set3DCameraActor(m_pGameCameraActor);

		m_pPlayer->GetPhysicsComponent()->SetWeight(1.f);
		dynamic_pointer_cast<PlayerMoveScript>(m_pPlayer->GetScriptList()[0])->CanInput();

		m_bStartBettyMoveScene = false;
		m_pCursor->m_bRender = true;
		m_bNoPaused = false;
	}

	// Player 움직임 구현
	m_pPlayer->SetPosition(m_cBettyMovie.GetPlayerPos());
	m_pPlayer->SetRotation(m_cBettyMovie.GetPlayerRot());
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
		break;
	case StagePhase::FINAL:
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

			if (COLLITION->CheckCollision(*iter, melee))
			{
				auto enemy = dynamic_pointer_cast<TEnemy>(*iter);
				if (enemy)
				{
					float damage = dynamic_pointer_cast<TPlayer>(m_pPlayer)->GetMeleeDamage();
					enemy->SetDamagedByM(damage);
				}
			}

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
			//COLLITION->CheckCollision(*proj, *iter);
			if (COLLITION->CheckCollision(*proj, *iter))
			{
				auto enemy = dynamic_pointer_cast<TEnemy>(*iter);
				if (enemy)
				{
					float damage = dynamic_pointer_cast<TPlayer>(m_pPlayer)->GetRangedDamage();
					enemy->SetDamagedByP(damage);
				}
			}
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
	StagePhase currentStage = STAGE->GetCurrentStage();
	
	auto& bloodList = EFFECT->GetBloodList();
	for (auto blood = bloodList.begin(); blood != bloodList.end(); )
	{
		shared_ptr<ABloodActor> pBlood = dynamic_pointer_cast<ABloodActor>(*blood);
		if (currentStage == StagePhase::FINAL)
		{
			pBlood->SetBlood(true);
		}

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

		for (auto iter = stage2.begin(); iter != stage2.end();)
		{
			if ((iter->get() == nullptr) || iter->get()->m_bDelete == true)
			{
				iter = stage2.erase(iter);
				continue;
			}
			COLLITION->CheckCollision(*blood, *iter);
			iter++;
		}
		

		blood++;
	}



	auto& soulList = EFFECT->GetSoulList();
	for (auto soul = soulList.begin(); soul != soulList.end(); )
	{
		if (!(*soul)->IsActive() || (*soul)->m_bDelete)
		{
			soul = soulList.erase(soul);
			continue;
		}

		if (auto pSoul = dynamic_pointer_cast<ASoulActor>(*soul))
		{
			Vec3 playerPos = m_pPlayer->GetPosition();
			playerPos.y += 1.5f;
			pSoul->SetTarget(playerPos);
		}

		if (COLLITION->CheckCollision(*soul, m_pPlayer))
		{
			//CheckEnemyDeath(enemyList);
			dynamic_pointer_cast<TPlayer>(m_pPlayer)->AddSoul(m_iSoulStorage);
			SOUND->GetPtr(ESoundType::GetSoul)->PlayEffect2D();
			m_iSoulStorage = 0;
		}
		soul++;
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
				//player->AddSoul(enemy->GetHisSoul());
				m_iSoulStorage += enemy->GetHisSoul();
			}
		}
	}
}

