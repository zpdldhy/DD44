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

	shared_ptr<class AActor> m_pGizmoCore;
	shared_ptr<class AActor> m_pGizmoX;
	shared_ptr<class AActor> m_pGizmoY;
	shared_ptr<class AActor> m_pGizmoZ;
	shared_ptr<class AActor> m_pSelectedGizmo;

	POINT m_vPrevMouse = {};
	Vec3 m_vDragStartPos = {};
	bool m_bDragging = false;

	bool m_bEnginCamera = false;

	// Asset loading
	ActorLoader actorLoader;
	MeshLoader meshLoader;

	float time;

	//shared_ptr<AActor> actor;
	shared_ptr<UMaterial> m_pSlashMaterial = nullptr;
	float m_fSlashTime = 0.0f;
	bool m_bSlashPlaying = false;
	float m_fSlashDuration = 0.5f; 

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
	void SetGizmoPosition(Vec3 _pos);

	void SetupEditorCallbacks();
	void SetupCharacterEditorCallback();
	void SetupMapEditorCallback();
	void SetupObjectEditorCallback();
	void SetupUIEditorCallback();
	void SetupParticleEditorCallback();

	void Slash();

	void SetClickPos();
	void LoadAllPrefabs(const std::string& extension);

};

