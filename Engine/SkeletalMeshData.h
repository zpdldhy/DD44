#pragma once

struct MeshComponentData
{
	wstring m_szComp;
	wstring m_szRes;
	int m_type = 0;
	bool m_bVisible = true;
	
	wstring m_szAnim;
	bool m_bHasAnim;
	int m_targetBone = 0;
	Matrix m_matBone;
	bool m_bInPlace = false;
	int m_rootIndex;
	
	wstring m_szTex;
	wstring m_szShader;

	Vec3 m_pos;
	Vec3 m_rot;
	Vec3 m_scale;

	vector<MeshComponentData> m_vChild;
};

struct MeshData
{
	bool m_bSkeleton = false;
	wstring m_szName;
	vector<PNCT_VERTEX> m_vVertexList;
	vector<IW_VERTEX> m_vIwList;
	vector<DWORD> m_vIndexList;
	UINT m_iMaterialIndex = -1;
	vector<Matrix> m_vInverseBindPose;
};

struct BoneNode
{
	UINT m_iIndex;
	wstring m_szName;
	wstring m_szParentName;
	vector<wstring> m_vChildName;
};

struct Skeleton
{
	map<wstring, BoneNode> m_vBonelist;
};

struct AnimTrackData
{
	wstring m_szName;
	UINT m_iStartFrame;
	UINT m_iEndFrame;

	vector<vector<Matrix>> m_vAnim;
};

struct AnimSequence
{
	vector<AnimTrackData> m_vAnimList;
};


