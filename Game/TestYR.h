#pragma once
#include "IExecute.h"

class TestYR : public IExecute
{
public:
	shared_ptr<class UStaticMeshComponent> m_pStaticMesh;
public:
	virtual void Init() override;
	virtual void Update() override;
	virtual void Render() override;
};

