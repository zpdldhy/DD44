#pragma once
#include "IExecute.h"

class ActorLoader;
class APawn;
class ACameraActor;
class UMeshComponent;

class TestPlayer : public IExecute
{
	shared_ptr<ActorLoader> loader;
	shared_ptr<APawn> player;
	shared_ptr<APawn> projectile;
	vector<shared_ptr<UMeshComponent>> meshList;

	shared_ptr<class ACameraActor> m_pCameraActor;
	vector<shared_ptr<APawn>> gizmo;

public:
	virtual void Init() override;
	virtual void Tick() override;
	virtual void Render() override;
	virtual void Destroy() override;
};

