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

	// UI
	vector<shared_ptr<class AUIActor>> m_vPausedBackGround;
	vector<shared_ptr<class AUIActor>> m_vUpgradeBackGround;
	vector<shared_ptr<class AUIActor>> m_vUpgradeState;
	vector<shared_ptr<class AUIActor>> m_vCoins;
	shared_ptr<AActor> m_pCursor = nullptr;

	// TEMP
	vector<shared_ptr<AActor>> enemyList; 
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
	void UpdateCursor();

protected:
	void SetEnemy();
	void CheckEnemyCollision();
public:
	Game() { m_type = SCENE_TYPE::GAME; }
};