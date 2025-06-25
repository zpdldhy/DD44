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

	// �浹
	vector<shared_ptr<AActor>> enemyList;
	// stage1�� �����
	vector<shared_ptr<AActor>> collisionEnemy1;
	
	vector<shared_ptr<AActor>> m_vObjectList;
	vector<shared_ptr<AActor>> m_vMapList;

	// �� ����
	BettyMeetControler m_cBettyMovie;
	bool m_bStartBettyMoveScene = false;	// �ݸ��� �ڽ��� ������ ������ �� �ֵ���

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