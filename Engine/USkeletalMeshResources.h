#pragma once
#include "UMeshResources.h"

class USkeletalMeshResources : public UMeshResources
{
public:
	USkeletalMeshResources() = default;
	virtual ~USkeletalMeshResources() = default;

protected:
	ComPtr<ID3D11Buffer> m_pIWBuffer;
	vector<IW_VERTEX> m_vIWList;
	
protected:
	bool CreateIWBuffer();

public:
	void Bind() override;
public:
	ComPtr<ID3D11Buffer> GetIwBuffer() { return m_pIWBuffer; }
	vector<IW_VERTEX> GetIwList() { return m_vIWList; }
	void SetIwList(vector<IW_VERTEX> _iwList) { m_vIWList = _iwList; }
};

