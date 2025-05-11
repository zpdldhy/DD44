#pragma once
#include "IExecute.h"
#include "Sound.h"

class UMaterial;

class TestSJ : public IExecute
{
private:
	std::shared_ptr<UMaterial> targetMat = nullptr;

public:
	virtual void Init() override;
	virtual void Update() override;
	virtual void Render() override;

public:
	shared_ptr<class UStaticMeshComponent> m_pStaticMesh;
	shared_ptr<class APawn> m_pActor;
	shared_ptr<class UStaticMeshComponent> m_pStaticMesh2;
	shared_ptr<class APawn> m_pActor2;

	shared_ptr<class ACameraActor> m_pCameraActor;
	shared_ptr<class UStaticMeshComponent> m_pSkyMesh;
	shared_ptr<class ASky> m_pSky;
	shared_ptr<class ALight> m_pLight;
	shared_ptr<class ALight> m_pPointLight;

	shared_ptr<class ActorLoader> m_pLoader;
	vector<shared_ptr<class UMeshComponent>> m_vMeshList;
	shared_ptr<class APawn> m_pSwordActor;

};

