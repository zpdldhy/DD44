#pragma once
#include "SkeletalMeshData.h"
#include <Fbx/fbxsdk.h>
#pragma comment(lib, "Fbx/libfbxsdk-md.lib")
#pragma comment(lib, "Fbx/libxml2-md.lib")
#pragma comment(lib, "Fbx/zlib-md.lib")

struct TIwData
{
	vector<UINT> m_vIndexList;
	vector<float> m_vWeightList;
	UINT Insert(UINT index, float weight)
	{
		m_vIndexList.emplace_back(index);
		m_vWeightList.emplace_back(weight);
		return m_vIndexList.size();
	}
};

struct TFbxResource
{
	wstring name;
	// Material
	map<int, wstring> m_mTexPathList;

	// AnimData
	vector<AnimTrackData> m_vAnimTrackList;

	// MESH DATA
	vector<vector<Matrix>> m_vInverseBindPose;
	vector<MeshData> m_vMeshList;

	// SKELETON
	map<int, BoneNode> m_mSkeletonList;

	// COUNT FOR FILE I/O
	UINT m_iBoneCount;
	UINT m_iTexPathCount;
	UINT m_iMeshCount;
	UINT m_iAnimTrackCount;
	UINT m_iNodeCount;

	// FOR INDEX
	UINT m_iBoneIndex = 0;

	void Clear()
	{
		m_mTexPathList.clear();
		m_vAnimTrackList.clear();
		m_vInverseBindPose.clear();
		m_vMeshList.clear();
		m_mSkeletonList.clear();
		m_iBoneCount = 0;
		m_iTexPathCount = 0;
		m_iMeshCount = 0;
		m_iAnimTrackCount = 0;
		m_iNodeCount = 0;
	}
};

//struct ParsingData
//{
//	TFbxResource m_ActorData;
//
//	vector<MeshData> m_vMeshList;
//	AnimSequence m_AnimData;
//	Skeleton m_Skeleton;
//	vector<wstring> m_vTexPathList;
//};

struct TempNode
{
	UINT m_iIndex;
	FbxNode* m_node;
	TempNode* m_parentNode;
	wstring m_szPatrentName;
	wstring m_szName;
	bool m_bLeaf = false;
	bool m_bMesh = false;
	bool m_bSkeleton = false;
};

class FbxLoader
{
public:
	FbxManager* m_pManager;
	FbxImporter* m_pImporter;
	FbxScene* m_pScene;
	FbxNode* m_pRootNode;

	TFbxResource m_result;
	vector<FbxNode*> m_vMeshes;
	map<wstring, TempNode> m_FbxBones;
	vector<TIwData> m_VertexWeights;

	Vec4 m_vRootPos;
	UINT m_iBoneIndex = 0;
	Vec4 m_vStaticRootPos;

	// Anim 
	int repeatThreshold = 2;
	int repeatCount = 0;
	int lastMeaningfulFrame = -1;
	int maxValidFrame = 0;

public:
	void Init(string _loadFile);
	void Destroy();

	TFbxResource Load(string _loadFile);
	void PreProcess(FbxNode* _node);
	void ParseMesh(FbxNode* _node);
	bool ParseSkinningMesh(FbxMesh* _mesh);
	void ParseAnimation();
	void GetAnimationTrack(int animTrack, int Count);
	void GetAnimation(int animTrack, AnimTrackData* track);
	vector<Matrix> GetNodeAnimation(FbxNode* node, AnimTrackData* track);

public:
	string ParseMaterial(FbxSurfaceMaterial* _pSurface);
	FbxColor ReadColor(FbxMesh* mesh, DWORD dwVertexColorCount, FbxLayerElementVertexColor* pVertexColorSet, DWORD dwDCCIndex, DWORD dwVertexIndex);
	FbxVector4 ReadNormal(const FbxMesh* mesh, DWORD dwVertexNormalCount, FbxLayerElementNormal* VertexNormalSets, int controlPointIndex, int iVertexIndex);
	void ReadTextureCoord(FbxMesh* pFbxMesh, FbxLayerElementUV* pUVSet, int vertexIndex, int uvIndex, FbxVector2& uv);
	int GetSubMaterialIndex(int iPoly, FbxLayerElementMaterial* pMaterialSetList);

public:
	Matrix DxConvertMatrix(Matrix m);
	Matrix ConvertAMatrix(FbxAMatrix& m);
};

