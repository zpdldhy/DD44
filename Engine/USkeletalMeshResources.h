#pragma once
#include "UMeshResources.h"

class USkeletalMeshResources : public UMeshResources
{
public:
	USkeletalMeshResources() = default;
	virtual ~USkeletalMeshResources() = default;

protected:
	ComPtr<ID3D11Buffer> m_pIWBuffer;
	//vector<IW_VERTEX> m_vIWList;
	
protected:
	bool CreateIWBuffer();

public:
	void Bind() override;
};

