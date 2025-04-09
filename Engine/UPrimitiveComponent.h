#pragma once
#include "USceneComponent.h"

class UMaterial;

class UPrimitiveComponent : public USceneComponent
{
protected:
	// MATERIAL
	shared_ptr<UMaterial> m_pMaterial;
	// MESH DATA
	ComPtr<ID3D11Buffer> m_pVertexBuffer;
	ComPtr<ID3D11Buffer> m_pIndexBuffer;
	vector<PNCT_VERTEX> m_vVertexList;
	vector<DWORD> m_vIndexList;

public:
	void SetMaterial(shared_ptr<UMaterial> _material);
	shared_ptr<UMaterial> GetMaterial();
public:
	bool CreateVertexBuffer();
	bool CreateIndexBuffer();
public:
	void Init() override;
	void Tick() override;
	void PreRender() override;
	void Render() override;
	void PostRender()override;
	void Destroy() override;
};

