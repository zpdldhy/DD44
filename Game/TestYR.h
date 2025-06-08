#pragma once
#include "IExecute.h"

class APawn;
class AActor;
class ACameraActor;
class UMeshComponent;
class MeshLoader;
class UStaticMeshComponent;

class TestYR : public IExecute
{
	shared_ptr <MeshLoader> meshLoader;
	shared_ptr<AActor> player;
	shared_ptr<AActor> boss_betty;
	vector<shared_ptr<AActor>> enemyList;
	shared_ptr<APawn> projectile;
	vector<shared_ptr<UMeshComponent>> meshList;

	shared_ptr<class ACameraActor> m_pGameCameraActor;
	shared_ptr<class ACameraActor> m_pCameraEngine;

	vector<shared_ptr<APawn>> gizmo;

	shared_ptr<UStaticMeshComponent> swordComp;
	shared_ptr<UStaticMeshComponent> backSocketComp;
	shared_ptr<UStaticMeshComponent> handSocketComp;
	shared_ptr<UMeshComponent> currentSwordParent;

	shared_ptr<AActor> plain;
	vector<shared_ptr<AActor>> m_vObjectList;

	bool m_bEnginCamera = true;
	int animIndex;
	Vec3 rot;

	Vec3 backSwordPos;
	Vec3 backSwordRot;

	Vec3 handSwordPos;
	Vec3 handSwordRot;

public:
	virtual void Init() override;
	virtual void Tick() override;
	virtual void Render() override;
	virtual void Destroy() override;
public:
	void SetupSunLight();
	void SetupEngineCamera();
	void SetupGameCamera();
	void CheckEnemyCollision();

};

