#pragma once
#define RUN_GAME
#include "IExecute.h"
#include "ActorLoader.h"
#include "MeshLoader.h"
#include "UIControler.h"

class Game : public IExecute
{
public:
	shared_ptr<class ACameraActor> m_pCameraActor;
	shared_ptr<class ACameraActor> m_pGameCameraActor;
	
	shared_ptr<class AActor> m_pPlayer;
	shared_ptr<class AActor> m_pBetty;
	shared_ptr<class AActor> tempHeadRoller;
	shared_ptr<class UStaticMeshComponent> m_pSkyMesh;
	shared_ptr<class ASky> m_pSky;
	shared_ptr<class ALight> m_pSunLight;

	bool m_bEnginCamera = false;
	bool m_bWind = false;

	float time;

	// UI
	InGameUIControler m_cUI;

	// UIActor
	shared_ptr<AActor> m_pCursor = nullptr;

	// TEMP
	vector<shared_ptr<AActor>> enemyList;
	//vector<shared_ptr<AActor>> enemyList0;
	//vector<shared_ptr<AActor>> enemyList1;
	//vector<shared_ptr<AActor>> enemyList2;
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
	void UpdateUI();
	void UpdateCursor();

protected:
	void SetEnemy(vector<shared_ptr<AActor>>& _enemyList);
	void CheckFrustumCulling();
	void CheckEnemyCollision();
	//blood
	void CheckBloodCollision();

public:
	Game() { m_type = SCENE_TYPE::GAME; }
};