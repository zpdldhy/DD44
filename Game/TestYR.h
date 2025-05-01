#pragma once
#include "IExecute.h"
#include "ActorLoader.h"

class APawn;

class TestYR : public IExecute
{
public:
	ActorLoader loader;
	vector<shared_ptr<APawn>> m_vActorList;

	shared_ptr<class ACameraActor> m_pCameraActor;
	
	vector<shared_ptr<APawn>> gizmo;

public:
	virtual void Init() override;
	virtual void Update() override;
	virtual void Render() override;
};

