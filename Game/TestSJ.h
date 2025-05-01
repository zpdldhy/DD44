#pragma once
#include "IExecute.h"
#include "Sound.h"

class TestSJ : public IExecute
{
public:
	virtual void Init() override;
	virtual void Update() override;
	virtual void Render() override;

public:
	shared_ptr<class UStaticMeshComponent> m_pStaticMesh;
	shared_ptr<class APawn> m_pActor;
	shared_ptr<class ACameraActor> m_pCameraActor;

	shared_ptr<class UStaticMeshComponent> m_pSkyMesh;
	shared_ptr<class ASky> m_pSky;
};

