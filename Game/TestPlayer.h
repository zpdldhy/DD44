#pragma once
#include "IExecute.h"

class APawn;
class ACameraActor;
class UMeshComponent;
class MeshLoader;

class TestPlayer : public IExecute
{
	shared_ptr <MeshLoader> meshLoader;
	shared_ptr<APawn> player;
	shared_ptr<APawn> projectile;
	vector<shared_ptr<UMeshComponent>> meshList;

	shared_ptr<class ACameraActor> m_pCameraActor;
	shared_ptr<class ACameraActor> m_pCameraEngine;
	shared_ptr<class ACameraActor> m_pGameCameraActor;


	vector<shared_ptr<APawn>> gizmo;

	shared_ptr<UMeshComponent> sword;
	shared_ptr<UMeshComponent> socket;

	bool m_bEnginCamera = true;
public:
	virtual void Init() override;
	virtual void Tick() override;
	virtual void Render() override;
	virtual void Destroy() override;
public:
	void SetupSunLight();
	void SetupEngineCamera();
	void SetupGameCamera();


public:
	void VisitAllMeshMaterials(shared_ptr<UMeshComponent> comp);
	float m_fHitFlashTimer = 0.0f;
	bool m_bIsFlashing = false;
	void ApplyHitFlashToAllMaterials(shared_ptr<UMeshComponent> comp, float value);

};

