#pragma once
#define RUN_GAME
#include "IExecute.h"
#include "ActorLoader.h"
#include "MeshLoader.h"

class Game : public IExecute
{
public:
	shared_ptr<class ACameraActor> m_pCameraActor;
	shared_ptr<class AActor> m_pPlayer;
	shared_ptr<class UStaticMeshComponent> m_pSkyMesh;
	shared_ptr<class ASky> m_pSky;
	shared_ptr<class ALight> m_pSunLight;

	bool m_bEnginCamera = false;

	float time;

	vector<shared_ptr<class AUIActor>> m_vHP;
	vector<shared_ptr<class AUIActor>> m_vArrow;

public:
	void Init() override;
	void Tick() override;
	void Render() override;
	void Destroy() override;

protected:
	void SetupEngineCamera();
	void SetupSkybox();
	void SetupSunLight();

public:
	Game() { m_type = SCENE_TYPE::GAME; }
};