#pragma once
#include "UMeshComponent.h"
#include "UAnimation.h"

class USkinnedComponent : public UMeshComponent
{
public:
	ComPtr<ID3D11Buffer> m_pIwBuffer;
	vector<IW_VERTEX> m_vIwList;
	shared_ptr<UAnimation> m_pAnim;
public:
	virtual void Init() override;
	virtual void Tick() override;
	//virtual void Render() override;
	virtual void Destroy() override;
public:
	virtual void PreRender() override;
	virtual void PostRender() override;
public:
	void CreateIwBuffer();
	void CreateCube();

};

