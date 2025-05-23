#pragma once
#include "IExecute.h"

class APawn;
class ACameraActor;
class UMeshComponent;
class MeshLoader;
class UStaticMeshComponent;

class TestYR : public IExecute
{
	shared_ptr <MeshLoader> meshLoader;
	shared_ptr<APawn> player;
	shared_ptr<APawn> projectile;
	vector<shared_ptr<UMeshComponent>> meshList;

	shared_ptr<class ACameraActor> m_pCameraActor;
	shared_ptr<class ACameraActor> m_pCameraEngine;

	vector<shared_ptr<APawn>> gizmo;

	shared_ptr<UMeshComponent> sword;
	shared_ptr<UStaticMeshComponent> backSocketComp;
	shared_ptr<UStaticMeshComponent> handSocketComp;


	bool m_bEnginCamera = true;
	int animIndex;
	Vec3 rot;

	Vec3 backSwordPos;
	Vec3 backSwordRot;

	Vec3 handSwordPos;
	Vec3 handSwordRot;

public:
	virtual void Init() override;
	virtual void Update() override;
	virtual void Render() override;
public:
	void SetupSunLight();
	void SetupEngineCamera();

};

