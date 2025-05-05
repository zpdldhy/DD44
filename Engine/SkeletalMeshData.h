#pragma once

struct MeshData
{
	bool m_bSkeleton = false;
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

struct SkeletonData
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