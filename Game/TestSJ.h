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

	Sound* m_pBomb = nullptr;
	Sound* m_pBgm = nullptr;
};

