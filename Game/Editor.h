#pragma once
#include "IExecute.h"
#include "ActorLoader.h"
#include "MeshLoader.h"

class Editor : public IExecute
{
public:
	shared_ptr<class ACameraActor> m_pCameraActor;

	shared_ptr<class AActor> m_pPlayer;

	shared_ptr<class UStaticMeshComponent> m_pSkyMesh;
	shared_ptr<class ASky> m_pSky;

	shared_ptr<class ALight> m_pSunLight;

	shared_ptr<class AActor> m_pGizmo;

	bool m_bEnginCamera = false;

	// Asset loading
	ActorLoader actorLoader;
	MeshLoader meshLoader;

	float time;

public:
	void Init() override;
	void Update() override;
	void Render() override;
	void Destroy();

protected:
	void SetupEngineCamera();
	void SetupSkybox();
	void SetupSunLight();
	void SetupGizmo();

	void SetupEditorCallbacks();
	void SetupCharacterEditorCallback();
	void SetupMapEditorCallback();
	void SetupObjectEditorCallback();

	void SetClickPos();
	void LoadAllPrefabs(const std::string& extension);

};

