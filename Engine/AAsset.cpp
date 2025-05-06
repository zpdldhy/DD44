#include "pch.h"
#include "AAsset.h"

void AAsset::Export(TFbxResource _result, string filepath)
{
	FILE* pFile;
	wstring fileName = SplitName(to_mw(filepath.c_str()));
	string path = "../Resources/Asset/";
	string ext = ".asset";
	path = path + to_wm(fileName.c_str()) + ext;
	errno_t err = fopen_s(&pFile, path.c_str(), "wb");
	if (err != 0) { assert(false); }

	// HEADER
	fwrite(&_result.m_iBoneCount, sizeof(UINT), 1, pFile);
	fwrite(&_result.m_iMeshCount, sizeof(UINT), 1, pFile);
	fwrite(&_result.m_iAnimTrackCount, sizeof(UINT), 1, pFile);
	fwrite(&_result.m_iTexPathCount, sizeof(UINT), 1, pFile);
	fwrite(&_result.m_iNodeCount, sizeof(UINT), 1, pFile);

	// TEX PATH
	{
		for (auto& iter : _result.m_mTexPathList)
		{
			UINT texPathNameSize = static_cast<UINT>(iter.second.size());
			fwrite(&texPathNameSize, sizeof(UINT), 1, pFile);
			fwrite(&iter.first, sizeof(int), 1, pFile);
			fwrite(iter.second.data(), sizeof(wchar_t), texPathNameSize, pFile);
		}
	}


	// ANIM DATA
	{
		for (int iAnimTrack = 0; iAnimTrack < _result.m_iAnimTrackCount; iAnimTrack++)
		{
			// NODE NAME
			UINT nameSize = static_cast<UINT>(_result.m_vAnimTrackList[iAnimTrack].m_szName.size());
			fwrite(&nameSize, sizeof(UINT), 1, pFile);
			fwrite(_result.m_vAnimTrackList[iAnimTrack].m_szName.data(), sizeof(wchar_t), nameSize, pFile);

			//
			fwrite(&_result.m_vAnimTrackList[iAnimTrack].m_iStartFrame, sizeof(UINT), 1, pFile);
			fwrite(&_result.m_vAnimTrackList[iAnimTrack].m_iEndFrame, sizeof(UINT), 1, pFile);

			UINT frameCount = _result.m_vAnimTrackList[iAnimTrack].m_vAnim[0].size();
			fwrite(&frameCount, sizeof(UINT), 1, pFile);
			for (int iBone = 0; iBone < _result.m_iNodeCount; iBone++)
			{
				fwrite(&_result.m_vAnimTrackList[iAnimTrack].m_vAnim[iBone].at(0), sizeof(Matrix), frameCount, pFile);
			}
		}
	}

	// BONE
	for (auto& data : _result.m_mSkeletonList)
	{
		// BONE NAME
		UINT nameSize = static_cast<UINT>(data.second.m_szName.size());
		fwrite(&nameSize, sizeof(UINT), 1, pFile);
		fwrite(data.second.m_szName.data(), sizeof(wchar_t), nameSize, pFile);

		// BONE DATA
		fwrite(&data.second.m_iIndex, sizeof(UINT), 1, pFile);
		UINT parentNameSize = static_cast<UINT>(data.second.m_szParentName.size());
		fwrite(&parentNameSize, sizeof(UINT), 1, pFile);
		fwrite(data.second.m_szParentName.data(), sizeof(wchar_t), parentNameSize, pFile);
	}

	// INVERSE MAT BONE
	for (int iMat = 0; iMat < _result.m_iMeshCount; iMat++)
	{
		fwrite(&_result.m_vInverseBindPose[iMat].at(0), sizeof(Matrix), _result.m_iNodeCount, pFile);
	}

	// MESH NODES
	for (int iMesh = 0; iMesh < _result.m_iMeshCount; iMesh++)
	{
		auto& child = _result.m_vMeshList[iMesh];
		// HEADER FOR NODE
		UINT nameSize = child.m_szName.size();
		fwrite(&nameSize, sizeof(UINT), 1, pFile);
		fwrite(child.m_szName.data(), sizeof(wchar_t), nameSize, pFile);

		fwrite(&child.m_bSkeleton, sizeof(bool), 1, pFile);
		UINT vertexSize = child.m_vVertexList.size();
		fwrite(&vertexSize, sizeof(UINT), 1, pFile);
		UINT iwSize = child.m_vIwList.size();
		fwrite(&iwSize, sizeof(UINT), 1, pFile);
		UINT indexSize = child.m_vIndexList.size();
		fwrite(&indexSize, sizeof(UINT), 1, pFile);
		fwrite(&child.m_iMaterialIndex, sizeof(UINT), 1, pFile);

		// VERTEX
		fwrite(&child.m_vVertexList.at(0), sizeof(PNCT_VERTEX), vertexSize, pFile);
		if (iwSize > 0)
		{
			fwrite(&child.m_vIwList.at(0), sizeof(IW_VERTEX), iwSize, pFile);
		}
		if (indexSize > 0)
		{
			fwrite(&child.m_vIndexList.at(0), sizeof(DWORD), indexSize, pFile);
		}
	}

	fclose(pFile);

}

TFbxResource AAsset::Load(const char* fileName)
{
	FILE* pFile;

	errno_t err = fopen_s(&pFile, fileName, "rb");
	if (err != 0) { assert(false); }
	TFbxResource result;
	result.name = SplitName(to_mw(fileName));

	// HEADER
	fread(&result.m_iBoneCount, sizeof(UINT), 1, pFile);
	fread(&result.m_iMeshCount, sizeof(UINT), 1, pFile);
	fread(&result.m_iAnimTrackCount, sizeof(UINT), 1, pFile);
	fread(&result.m_iTexPathCount, sizeof(UINT), 1, pFile);
	fread(&result.m_iNodeCount, sizeof(UINT), 1, pFile);

	//TEX PATH
	for (int iTex = 0; iTex < result.m_iTexPathCount; iTex++)
	{
		int texPathNameSize;
		fread(&texPathNameSize, sizeof(UINT), 1, pFile);
		int texPathIndex;
		wstring texPath;
		texPath.resize(texPathNameSize);
		fread(&texPathIndex, sizeof(int), 1, pFile);
		fread(&texPath[0], sizeof(wchar_t), texPathNameSize, pFile);

		result.m_mTexPathList.insert(make_pair(texPathIndex, texPath));
	}

	// ANIM DATA
	result.m_vAnimTrackList.resize(result.m_iAnimTrackCount);
	for (int iAnimTrack = 0; iAnimTrack < result.m_iAnimTrackCount; iAnimTrack++)
	{
		// NODE NAME
		int nameSize = 0;
		fread(&nameSize, sizeof(UINT), 1, pFile);
		result.m_vAnimTrackList[iAnimTrack].m_szName.resize(nameSize);
		fread(&result.m_vAnimTrackList[iAnimTrack].m_szName[0], sizeof(wchar_t), nameSize, pFile);

		fread(&result.m_vAnimTrackList[iAnimTrack].m_iStartFrame, sizeof(UINT), 1, pFile);
		fread(&result.m_vAnimTrackList[iAnimTrack].m_iEndFrame, sizeof(UINT), 1, pFile);


		UINT frameCount = 0;
		fread(&frameCount, sizeof(UINT), 1, pFile);

		result.m_vAnimTrackList[iAnimTrack].m_vAnim.resize(result.m_iNodeCount);
		for (int iBone = 0; iBone < result.m_iNodeCount; iBone++)
		{
			result.m_vAnimTrackList[iAnimTrack].m_vAnim[iBone].resize(frameCount);
			fread(&result.m_vAnimTrackList[iAnimTrack].m_vAnim[iBone].at(0), sizeof(Matrix), frameCount, pFile);
		}
	}

	// BONE
	for (int iBone = 0; iBone < result.m_iBoneCount; iBone++)
	{
		BoneNode bone;
		// BONE NAME
		UINT nameSize;
		fread(&nameSize, sizeof(UINT), 1, pFile);
		bone.m_szName.resize(nameSize);
		fread(&bone.m_szName[0], sizeof(wchar_t), nameSize, pFile);

		// BONE DATA
		fread(&bone.m_iIndex, sizeof(UINT), 1, pFile);

		UINT parentNameSize;
		fread(&parentNameSize, sizeof(UINT), 1, pFile);
		bone.m_szParentName.resize(parentNameSize);
		fread(&bone.m_szParentName[0], sizeof(wchar_t), parentNameSize, pFile);

		result.m_mSkeletonList.insert(make_pair(bone.m_iIndex, bone));
	}


	// INVERSE MAT BONE
	result.m_vInverseBindPose.resize(result.m_iMeshCount);
	for (int iMat = 0; iMat < result.m_iMeshCount; iMat++)
	{
		result.m_vInverseBindPose[iMat].resize(result.m_iNodeCount);
		fread(&result.m_vInverseBindPose[iMat].at(0), sizeof(Matrix), result.m_iNodeCount, pFile);
	}

	// MESH NODES
	result.m_vMeshList.resize(result.m_iMeshCount);
	for (int iMesh = 0; iMesh < result.m_iMeshCount; iMesh++)
	{
		auto& child = result.m_vMeshList[iMesh];
		// HEADER FOR NODE
		UINT nameSize;
		fread(&nameSize, sizeof(UINT), 1, pFile);
		child.m_szName.resize(nameSize);
		fread(&child.m_szName[0], sizeof(wchar_t), nameSize, pFile);
		
		fread(&child.m_bSkeleton, sizeof(bool), 1, pFile);
		UINT vertexSize = 0;
		fread(&vertexSize, sizeof(UINT), 1, pFile);
		UINT iwSize = 0;
		fread(&iwSize, sizeof(UINT), 1, pFile);
		UINT indexSize = 0;
		fread(&indexSize, sizeof(UINT), 1, pFile);
		fread(&child.m_iMaterialIndex, sizeof(UINT), 1, pFile);

		// VERTEX
		child.m_vVertexList.resize(vertexSize);
		fread(&child.m_vVertexList.at(0), sizeof(PNCT_VERTEX), vertexSize, pFile);
		if (iwSize > 0)
		{
			child.m_vIwList.resize(iwSize);
			fread(&child.m_vIwList.at(0), sizeof(IW_VERTEX), iwSize, pFile);
		}
		if (indexSize > 0)
		{
			child.m_vIndexList.resize(indexSize);
			fread(&child.m_vIndexList.at(0), sizeof(DWORD), indexSize, pFile);
		}
	}
	
	fclose(pFile);
	return result;
}
