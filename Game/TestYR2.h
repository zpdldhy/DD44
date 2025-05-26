#pragma once
#include "IExecute.h"
#include "ImGuiCore.h"
#include "MeshLoader.h"

class ActorLoader;
class ObjectLoader;
class UMeshComponent;
class UMeshResources;
class UAnimInstance;
class ACameraActor;
class APawn;
class AActor;

class TestYR2 : public IExecute
{
	shared_ptr<ActorLoader> loader;
	shared_ptr<ObjectLoader> objLoader;
	MeshLoader meshLoader;

	vector<shared_ptr<UMeshComponent>> meshList;
	map<wstring, shared_ptr<UMeshResources>> meshResMap;
	vector<shared_ptr<UAnimInstance>> animList;
	vector<wstring> texList;

	vector<shared_ptr<AActor>> gizmo;
	shared_ptr<ACameraActor> m_pCameraActor;
	vector<shared_ptr<APawn>> m_vActorList;
	bool bSaveMesh;
	shared_ptr<APawn> m_pActor;
	string name;
	int animIndex = 0;

public:
	virtual void Init() override;
	virtual void Update() override;
	virtual void Render() override;
public:
	void AddChild(PreMeshData data);
	void SetupLight();
	void SetupGizmo();
};

