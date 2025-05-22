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
	vector<shared_ptr<APawn>> gizmo;

	shared_ptr<UMeshComponent> sword;
	shared_ptr<UMeshComponent> socket;
public:
	virtual void Init() override;
	virtual void Update() override;
	virtual void Render() override;
public:
	void SetupSunLight();

};

