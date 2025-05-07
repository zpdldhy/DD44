#pragma once
#include "IExecute.h"

class ActorLoader;
class APawn;
class ACameraActor;

class TestPlayer : public IExecute
{
	shared_ptr<ActorLoader> loader;
	shared_ptr<APawn> player;

	shared_ptr<class ACameraActor> m_pCameraActor;
	vector<shared_ptr<APawn>> gizmo;

public:
	virtual void Init() override;
	virtual void Update() override;
	virtual void Render() override;

};

