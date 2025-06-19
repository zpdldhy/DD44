#pragma once
#define RUN_GAME
#include "IExecute.h"
#include "ActorLoader.h"
#include "MeshLoader.h"

class Game : public IExecute
{
public:
	shared_ptr<class ACameraActor> m_pCameraActor;
	shared_ptr<class ACameraActor> m_pGameCameraActor;
	
	shared_ptr<class AActor> m_pPlayer;
	shared_ptr<class AActor> m_pBetty;
	shared_ptr<class UStaticMeshComponent> m_pSkyMesh;
	shared_ptr<class ASky> m_pSky;
	shared_ptr<class ALight> m_pSunLight;

	bool m_bEnginCamera = false;
	bool m_bWind = false;

	float time;

	// UIActor
	shared_ptr<AActor> m_pCursor = nullptr;
	// InGame
	vector<shared_ptr<class AUIActor>> m_vHPUI;
	Color fullHP = { 0.055f, 0.247f, -0.324, 0.0f };
	int m_iPreHP = 0;
	bool m_bHPUIChange = false;

	vector<shared_ptr<class AUIActor>> m_vArrowUI;
	shared_ptr<class Texture> m_pActiveArrowTexture = nullptr;
	shared_ptr<class Texture> m_pInActiveArrowTexture = nullptr;
	Vec3 m_vActiveArrowScale = { 0.f, 0.f, 0.f };
	Vec3 m_vInActiveArrowScale = { 0.f, 0.f, 0.f };
	// Paused
	vector<shared_ptr<class AUIActor>> m_vPausedBackGround;
	vector<shared_ptr<class AUIActor>> m_vUpgradeBackGround;
	vector<shared_ptr<class AUIActor>> m_vUpgradeState;
	vector<shared_ptr<class AUIActor>> m_vCoins;
	// Dead
	shared_ptr<class AUIActor> m_pDeadUI = nullptr;
	float m_fDeadUIPopTime = 1.f;

	// TEMP
	vector<shared_ptr<AActor>> enemyList1;
	vector<shared_ptr<AActor>> enemyList2;
	vector<shared_ptr<AActor>> m_vObjectList;
	vector<shared_ptr<AActor>> m_vMapList;

public:
	void Init() override;
	void Tick() override;
	void Render() override;
	void Destroy() override;

protected:
	void SetupEngineCamera();
	void SetupGameCamera();
	void SetupSkybox();
	void SetupSunLight();
	void CreateWind();
	void CreateUI();
	void UpdateUI();
	void UpdateCursor();

protected:
	void SetEnemy(vector<shared_ptr<AActor>>& _enemyList);
	void CheckEnemyCollision();

public:
	Game() { m_type = SCENE_TYPE::NON_GAME; }
};