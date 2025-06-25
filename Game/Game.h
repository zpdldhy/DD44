#pragma once
#define RUN_GAME
#include "IExecute.h"
#include "ActorLoader.h"
#include "MeshLoader.h"
#include "UIControler.h"
#include "MovieControler.h"

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

	// UI
	InGameUIControler m_cUI;

	// UIActor
	shared_ptr<AActor> m_pCursor = nullptr;

	// 충돌
	vector<shared_ptr<AActor>> enemyList;
	// stage1의 잡몹들
	vector<shared_ptr<AActor>> collisionEnemy1;
	
	vector<shared_ptr<AActor>> m_vObjectList;
	vector<shared_ptr<AActor>> m_vMapList;

	// 씬 연출
	BettyMeetControler m_cBettyMovie;
	bool m_bStartBettyMoveScene = false;	// 콜리전 박스에 닿으면 실행할 수 있도록

	// TEMP
	vector<shared_ptr<AActor>> stage0;
	vector<shared_ptr<AActor>> stage1;
	vector<shared_ptr<AActor>> stage2;


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

	void BettyMeetMovie();

protected:
	void SetEnemy(vector<shared_ptr<AActor>>& _enemyList);
	void CheckFrustumCulling();
	void CheckEnemyCollision();
	//blood
	void CheckBloodCollision();

public:
	Game() { m_type = SCENE_TYPE::GAME; }
};