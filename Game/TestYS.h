#pragma once
#include "IExecute.h"
#include "CameraActor.h"
#include "UStaticMeshComponent.h"

class TestYS : public IExecute
{
	shared_ptr<CameraActor> m_pCamera;
	shared_ptr<AActor> m_pObject;

public:
	virtual void Init() override;
	virtual void Update() override;
	virtual void Render() override;
};

