#pragma once

struct MeshComponentData
{
	wstring m_szComp = L"";
	wstring m_szRes = L"";
	int m_type = 0;
	bool m_bVisible = true;
	
	wstring m_szAnim = L"";
	bool m_bHasAnim = false;		// 초기화 추가
	int m_targetBone = 0;
	Matrix m_matBone = Matrix();
	bool m_bInPlace = false;
	int m_rootIndex = 0;
	
	wstring m_szTex = L"";
	wstring m_szShader = L"";

	Vec3 m_pos = Vec3();
	Vec3 m_rot = Vec3();
	Vec3 m_scale = Vec3();

	vector<MeshComponentData> m_vChild;
};

struct MeshData
{
	bool m_bSkeleton = false;
	wstring m_szName = L"";
	vector<PNCT_VERTEX> m_vVertexList;
	vector<IW_VERTEX> m_vIwList;
	vector<DWORD> m_vIndexList;
	UINT m_iMaterialIndex = -1;
	vector<Matrix> m_vInverseBindPose;
};

struct BoneNode
{
	UINT m_iIndex = 0;
	wstring m_szName = L"";
	wstring m_szParentName = L"";
	vector<wstring> m_vChildName;
};

struct Skeleton
{
	map<wstring, BoneNode> m_vBonelist;
};

struct AnimTrackData
{
	wstring m_szName = L"";
	UINT m_iStartFrame = 0;
	UINT m_iEndFrame = 0;

	vector<vector<Matrix>> m_vAnim;

	vector<XMFLOAT4> m_vAnimList;
};

struct AnimSequence
{
	vector<AnimTrackData> m_vAnimList;
};


