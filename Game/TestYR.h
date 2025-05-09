#pragma once
#include "IExecute.h"
#include "ActorLoader.h"
#include "ObjectLoader.h"

class APawn;
class UAnimInstance;

class TestYR : public IExecute
{
public:
	ActorLoader loader;
	ObjectLoader objLoader;
	vector<shared_ptr<AActor>> m_vActorList;
	vector<shared_ptr<APawn>> m_vObjList;
	vector<shared_ptr<UMaterial>> materialList;
	shared_ptr<AActor> targetObj;

	vector<shared_ptr<AActor>> m_vImGuiList;

	
	//// actor에 넣을 컴포넌트들 준비
	vector<shared_ptr<UMeshComponent>> meshList;
	vector<shared_ptr<UMeshComponent>> tempmeshList;
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
};

