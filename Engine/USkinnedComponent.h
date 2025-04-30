#pragma once
#include "UMeshComponent.h"

class USkinnedComponent : public UMeshComponent
{
public:
	ComPtr<ID3D11Buffer> m_pIwBuffer;
	vector<IW_VERTEX> m_vIwList;
public:
	//virtual void Init();
	//virtual void Tick();
	//virtual void Render();
	//virtual void Destroy();
public:
	virtual void PreRender() override;
	virtual void PostRender() override;
public:
	//void CreateIndexBuffer();
	void CreateIwBuffer();

};

