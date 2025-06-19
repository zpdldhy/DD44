#pragma once
#include "UMeshResources.h"
#include "SkeletalMeshData.h"

struct CBBindPoseData
{
	Matrix inverseBindPose[250];
};

class USkeletalMeshResources : public UMeshResources
{
public:
	USkeletalMeshResources() { m_Type = MeshType::M_SKINNED; }
	virtual ~USkeletalMeshResources() = default;

protected:
	ComPtr<ID3D11Buffer> m_pIWBuffer;
	vector<IW_VERTEX> m_vIWList;
	CBBindPoseData m_inverseBindPose;
	ComPtr<ID3D11Buffer> m_pBindPoseCB;
	map<wstring, BoneNode> m_vSkeletonList;

protected:
	bool CreateIWBuffer();
	void CreateConstantBuffer();

public:
	void Create() override;
	void Bind() override;
	void UpdateBindPoseData();

public:
	ComPtr<ID3D11Buffer> GetIwBuffer() { return m_pIWBuffer; }
	vector<IW_VERTEX> GetIwList() { return m_vIWList; }
	void SetIwList(vector<IW_VERTEX> _iwList) { m_vIWList = _iwList; }
	void AddSkeleton(map<wstring, BoneNode> _boneList) { m_vSkeletonList = _boneList; }
	void SetInverseBindPose(vector<Matrix> _inverseBindPose);
	int GetBoneIndex(wstring _name);
	const std::map<std::wstring, BoneNode>& GetSkeletonList() const
	{
		return m_vSkeletonList;
	}
};

