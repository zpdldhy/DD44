#pragma once
#include "IExecute.h"

class ActorLoader;
class UMeshComponent;
class UMeshResources;
class UAnimInstance;
class ACameraActor;
class APawn;
class AActor;

class TestYR2 : public IExecute
{
	shared_ptr<ActorLoader> loader;

	vector<shared_ptr<UMeshComponent>> meshList;
	vector<shared_ptr<UMeshResources>> meshResList;
	vector<shared_ptr<UAnimInstance>> animList;
	vector<wstring> texList;

	vector<shared_ptr<AActor>> gizmo;
	shared_ptr<ACameraActor> m_pCameraActor;
	vector<shared_ptr<APawn>> m_vActorList;


public:
	virtual void Init() override;
	virtual void Update() override;
	virtual void Render() override;
};

