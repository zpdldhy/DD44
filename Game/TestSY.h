#pragma once
#include "IExecute.h"

class TestSY : public IExecute
{
public:
	shared_ptr<class UStaticMeshComponent> m_pStaticMesh;
	shared_ptr<class AActor> m_pActor;

public:
	void Init() override;
	void Update() override;
	void Render() override;
	void Destroy();

public:

};

