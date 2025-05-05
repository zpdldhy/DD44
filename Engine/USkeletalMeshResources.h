#pragma once
#include "UMeshResources.h"
#include "SkeletalMeshData.h"

class USkeletalMeshResources : public UMeshResources
{
public:
	USkeletalMeshResources() = default;
	virtual ~USkeletalMeshResources() = default;

protected:
	ComPtr<ID3D11Buffer> m_pIWBuffer;
	vector<IW_VERTEX> m_vIWList;
	map<wstring, BoneNode> m_vSkeletonList;
	
protected:
	bool CreateIWBuffer();

public:
	void Create() override;
	void Bind() override;

public:
	ComPtr<ID3D11Buffer> GetIwBuffer() { return m_pIWBuffer; }
	vector<IW_VERTEX> GetIwList() { return m_vIWList; }
	void SetIwList(vector<IW_VERTEX> _iwList) { m_vIWList = _iwList; }
	void AddSkeleton(map<wstring, BoneNode> _boneList) { m_vSkeletonList = _boneList; }
	int GetBoneIndex(wstring _name);
};

