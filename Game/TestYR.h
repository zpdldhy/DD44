#pragma once
#include "IExecute.h"
#include "ActorLoader.h"
#include "ObjectLoader.h"

class APawn;
class UAnimInstance;
class UMeshResources;

class TestYR : public IExecute
{
public:
	ActorLoader loader;
	ObjectLoader objLoader;
	vector<shared_ptr<APawn>> m_vActorList;
	shared_ptr<APawn> m_pActor;
	shared_ptr<APawn> m_pActor2;
	vector<shared_ptr<APawn>> m_vObjList;
	vector<shared_ptr<UMaterial>> materialList;
	shared_ptr<AActor> targetObj;

	//// actor에 넣을 컴포넌트들 준비
	map<wstring, shared_ptr<UMeshResources>> meshMap;
	vector<shared_ptr<UMeshComponent>> tempmeshList;
	map<wstring, shared_ptr<UAnimInstance>> animMap;
	vector<wstring> texPathList;
	vector<shared_ptr<UAnimInstance>> animInstanceList;


	int targetIndex = 0;
	int matIndex = 0;

	shared_ptr<class ACameraActor> m_pCameraActor;
	
	vector<shared_ptr<AActor>> gizmo;

public:
	virtual void Init() override;
	virtual void Update() override;
	virtual void Render() override;
public:
	shared_ptr<UMeshComponent> MakeMC(MeshComponentData data, bool bRoot, shared_ptr<UAnimInstance> anim);
};

