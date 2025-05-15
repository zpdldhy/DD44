#include "pch.h"
#include "FbxLoader.h"

void FbxLoader::Init(string _loadFile)
{
	m_pManager = FbxManager::Create();
	m_pImporter = FbxImporter::Create(m_pManager, "");
	m_pScene = FbxScene::Create(m_pManager, "");

	FbxAxisSystem::MayaZUp.ConvertScene(m_pScene);

	if (!m_pImporter->Initialize(_loadFile.c_str()))
	{
		Destroy();
		assert(false);
	}

	if (!m_pImporter->Import(m_pScene))
	{
		Destroy();
		assert(false);
	}

	m_pRootNode = m_pScene->GetRootNode();
}

void FbxLoader::Destroy()
{
	if (m_pScene)		m_pScene->Destroy();
	if (m_pImporter)	m_pImporter->Destroy();
	if (m_pManager)		m_pManager->Destroy();

	m_pScene = nullptr;
	m_pImporter = nullptr;
	m_pManager = nullptr;

	m_vMeshes.clear();
	m_FbxBones.clear();
	m_VertexWeights.clear();

	m_iBoneIndex = 0;

}

TFbxResource FbxLoader::Load(string _loadFile)
{
	Init(_loadFile);

	m_result.Clear();

	// 본 파싱 및 메시 리스트화
	PreProcess(m_pRootNode);
	m_result.m_iBoneCount = m_result.m_mSkeletonList.size();
	m_result.m_iMeshCount = m_vMeshes.size();
	m_result.m_iNodeCount = m_result.m_iMeshCount + m_result.m_iBoneCount;

	// mesh 처리
	for (auto& mesh : m_vMeshes)
	{
		ParseMesh(mesh);
	}

	ParseAnimation();

	Destroy();
	return m_result;
}

void FbxLoader::PreProcess(FbxNode* _node)
{
	// crow_final 저장
	if (!strcmp(_node->GetName(),"crow_final"))
	{
		int a = 0;
		FbxVector4 rootP = _node->EvaluateGlobalTransform(0).GetT();

		m_vStaticRootPos.x = -rootP[0];
		m_vStaticRootPos.y = -rootP[1];
		m_vStaticRootPos.z = -rootP[2];
	}
	
	// skeleton인지 확인
	if (_node->GetNodeAttribute() && _node->GetNodeAttribute()->GetAttributeType() == FbxNodeAttribute::eSkeleton)
	{
		BoneNode bone;
		wstring name = to_mw(_node->GetName());
		bone.m_iIndex = m_iBoneIndex++;
		wstring parentName = L"None";
		if (auto parent = _node->GetParent())
		{
			parentName = to_mw(parent->GetName());
		}
		bone.m_szParentName = parentName;
		bone.m_szName = name;
		m_result.m_mSkeletonList.insert(make_pair(bone.m_iIndex, bone));
		TempNode temp;
		temp.m_iIndex = bone.m_iIndex;
		temp.m_node = _node;
		m_FbxBones.insert(make_pair(bone.m_szName, temp));
	}
	// mesh인지 확인
	if (_node->GetMesh())
	{
		m_vMeshes.emplace_back(_node);

		// TEMP
		wstring name = to_mw(_node->GetName());
		wstring parentName = L"None";
		if (auto parent = _node->GetParent())
		{
			parentName = to_mw(parent->GetName());
		}
		TempNode temp;
		temp.m_node = _node;
		temp.m_szPatrentName = parentName;
		m_FbxBones.insert(make_pair(name, temp));
	}

	int iNumChild = _node->GetChildCount();

	for (int i = 0; i < iNumChild; i++)
	{
		FbxNode* pChild = _node->GetChild(i);

		PreProcess(pChild);
	}
}

void FbxLoader::ParseMesh(FbxNode* _node)
{
	MeshData retMesh;
	FbxMesh* mesh = _node->GetMesh();
	retMesh.m_bSkeleton = ParseSkinningMesh(mesh);
	retMesh.m_vInverseBindPose = m_result.m_vInverseBindPose[m_result.m_vInverseBindPose.size() - 1];
	retMesh.m_szName = to_mw(_node->GetName());

	// 기하행렬 ( 초기 정점 위치를 변환할 떄 사용 ) 
	FbxNode* pNode = mesh->GetNode();
	FbxAMatrix globalTransform = pNode->EvaluateGlobalTransform(0);

	FbxAMatrix geom;
	FbxVector4 trans = pNode->GetGeometricTranslation(FbxNode::eSourcePivot);
	FbxVector4 rot = pNode->GetGeometricRotation(FbxNode::eSourcePivot); 
	FbxVector4 scale = pNode->GetGeometricScaling(FbxNode::eSourcePivot);
	geom.SetT(trans);
	geom.SetR(rot);
	geom.SetS(scale);

	FbxAMatrix normalMatrix = geom;
	normalMatrix = normalMatrix.Inverse();
	normalMatrix = normalMatrix.Transpose();

	if (!retMesh.m_bSkeleton)
	{
		geom = globalTransform * geom;
	}
	// 레이어
	vector<FbxLayerElementUV*> VertexUVSet;
	vector<FbxLayerElementVertexColor*> VertexColorSet;
	vector<FbxLayerElementTangent*> VertexTangentSet;
	vector<FbxLayerElementMaterial*> VertexMaterialSet;
	vector<FbxLayerElementNormal*> VertexNormalSet;
	// 레이어 처리
	int iLayderCount = mesh->GetLayerCount();

	for (int i = 0; i < iLayderCount; i++)
	{
		FbxLayer* pFbxLayer = mesh->GetLayer(i);
		if (pFbxLayer->GetUVs() != nullptr)
		{
			VertexUVSet.push_back(pFbxLayer->GetUVs());
		}
		if (pFbxLayer->GetVertexColors() != nullptr)
		{
			VertexColorSet.push_back(pFbxLayer->GetVertexColors());
		}
		if (pFbxLayer->GetNormals() != nullptr)
		{
			VertexNormalSet.push_back(pFbxLayer->GetNormals());
		}
		if (pFbxLayer->GetMaterials() != nullptr)
		{
			VertexMaterialSet.push_back(pFbxLayer->GetMaterials());
		}
	}

	// 텍스쳐 경로 파싱
	int iNumMtl = pNode->GetMaterialCount();
	for (int i = 0; i < iNumMtl; i++)
	{
		FbxSurfaceMaterial* pSurface = pNode->GetMaterial(i);
		if (pSurface)
		{
			string texPath = ParseMaterial(pSurface);
			if (!texPath.empty())
			{
				m_result.m_mTexPathList.insert(make_pair(i, (to_mw(texPath))));
			}
		}
	}
	m_result.m_iTexPathCount = m_result.m_mTexPathList.size();

	int iNumPolyCount = mesh->GetPolygonCount();
	FbxVector4* pVertexPositions = mesh->GetControlPoints();
	int iBasePolyIndex = 0;
	for (int i = 0; i < iNumPolyCount; i++)
	{
		int iPolySize = mesh->GetPolygonSize(i);
		int iNumFace = iPolySize - 2;
		for (int iFace = 0; iFace < iNumFace; iFace++)
		{
			int VertexIndex[3] = { 0, iFace + 2, iFace + 1 };

			// 정점위치 인덱스
			int cornerIndex[3];
			cornerIndex[0] = mesh->GetPolygonVertex(i, VertexIndex[0]);
			cornerIndex[1] = mesh->GetPolygonVertex(i, VertexIndex[1]);
			cornerIndex[2] = mesh->GetPolygonVertex(i, VertexIndex[2]);

			// 텍스쳐좌표 인덱스
			int uvIndex[3];
			uvIndex[0] = mesh->GetTextureUVIndex(i, 0);
			uvIndex[1] = mesh->GetTextureUVIndex(i, iFace + 2);
			uvIndex[2] = mesh->GetTextureUVIndex(i, iFace + 1);

			// PNCT로 변환
			for (int index = 0; index < 3; index++)
			{
				PNCT_VERTEX v;
				// POSITION
				FbxVector4 pPos = pVertexPositions[cornerIndex[index]];
				// 행렬을 곱한다 ( 오른손 좌표계라 행렬 * 정점 으로 정의된 MultT 함수 ) 
				auto p = geom.MultT(pPos);

				v.pos.x = p.mData[0];
				v.pos.y = p.mData[2];
				v.pos.z = p.mData[1];

				// COLOR
				FbxColor color = FbxColor(1, 1, 1, 1);
				if (VertexColorSet.size() > 0)
				{
					color = ReadColor(mesh, VertexColorSet.size(), VertexColorSet[0], cornerIndex[index], iBasePolyIndex + VertexIndex[index]);
				}
				v.color.x = color.mRed;
				v.color.y = color.mGreen;
				v.color.z = color.mBlue;
				v.color.w = 1.0f;

				// NORMAL
				if (VertexNormalSet.size() > 0)
				{
					FbxVector4 finalNorm = ReadNormal(mesh, VertexNormalSet.size(), VertexNormalSet[0], cornerIndex[index], iBasePolyIndex + VertexIndex[index]);
					finalNorm.mData[3] = 0.0f;
					finalNorm = normalMatrix.MultT(finalNorm);
					finalNorm.Normalize();
					v.normal.x = finalNorm.mData[0];
					v.normal.y = finalNorm.mData[2];
					v.normal.z = finalNorm.mData[1];
				}

				// UV
				if (VertexUVSet.size() > 0)
				{
					FbxLayerElementUV* pUVSet = VertexUVSet[0];
					FbxVector2 uv;
					ReadTextureCoord(mesh, pUVSet, cornerIndex[index], uvIndex[index], uv);

					v.uv.x = uv.mData[0];
					v.uv.y = 1.0f - uv.mData[1];
				}

				IW_VERTEX iw;
				//iw.i[0] = temp->index;
				//iw.w[0] = 1;
				if (retMesh.m_bSkeleton)
				{
					//TIwData& weight = m_VertexWeights[cornerIndex[index]];
					for (int iWeight = 0; iWeight < m_VertexWeights[cornerIndex[index]].m_vIndexList.size(); iWeight++)
					{
						if (iWeight > 3)
						{
							iw.i2[iWeight - 4] = m_VertexWeights[cornerIndex[index]].m_vIndexList[iWeight];
							iw.w2[iWeight - 4] = m_VertexWeights[cornerIndex[index]].m_vWeightList[iWeight];
						}
						else
						{
							iw.i1[iWeight] = m_VertexWeights[cornerIndex[index]].m_vIndexList[iWeight];
							iw.w1[iWeight] = m_VertexWeights[cornerIndex[index]].m_vWeightList[iWeight];
						}
					}
				}

				int iSubMaterialIndex = 0;
				if (VertexMaterialSet.size() > 0)
				{
					iSubMaterialIndex = GetSubMaterialIndex(i, VertexMaterialSet[0]);
				}

				retMesh.m_iMaterialIndex = iSubMaterialIndex;
				retMesh.m_vVertexList.emplace_back(v);
				retMesh.m_vIwList.emplace_back(iw);
			}
		}

		iBasePolyIndex += iPolySize;
	}

	//m_result.fbxNode.subMatSize = retNode.m_subMats.size();
	m_result.m_vMeshList.emplace_back(retMesh);
}

bool FbxLoader::ParseSkinningMesh(FbxMesh* _mesh)
{
	vector<Matrix> inverseBindPose;
	inverseBindPose.resize(m_result.m_iNodeCount);
	bool bRet = false;

	int deformerCount = _mesh->GetDeformerCount();
	if (deformerCount <= 0)
	{
		// static mesh 라면 단위행렬 벡터 반환
		m_result.m_vInverseBindPose.emplace_back(inverseBindPose);
		return bRet;
	}
	int iVertexCount = _mesh->GetControlPointsCount();
	m_VertexWeights.clear();
	m_VertexWeights.resize(iVertexCount);

	for (int i = 0; i < deformerCount; i++)
	{
		FbxSkin* pSkin = (FbxSkin*)_mesh->GetDeformer(i, FbxDeformer::eSkin);
		if (pSkin == nullptr) { continue; }
		int clusterCount = pSkin->GetClusterCount();
		for (int j = 0; j < clusterCount; j++)
		{
			// cluster와 link 얻기
			FbxCluster* cluster = pSkin->GetCluster(j);
			if (cluster == nullptr) { continue; }
			FbxNode* link = cluster->GetLink();
			if (link == nullptr) { continue; }

			// 본 인덱스 구하기
			UINT boneIndex = 0;
			wstring linkName = to_mw(link->GetName());
			auto iter = m_FbxBones.find(linkName);
			if (iter != m_FbxBones.end())
			{
				boneIndex = iter->second.m_iIndex;
			}

			// 가중치 구하기
			int iClusterSize = cluster->GetControlPointIndicesCount();
			int* pFbxNodeIndex = cluster->GetControlPointIndices();
			double* pFbxNodeWegiht = cluster->GetControlPointWeights();
			for (int v = 0; v < iClusterSize; v++)
			{
				bRet = true;
				int iIndex = pFbxNodeIndex[v];
				float fWeight = pFbxNodeWegiht[v];
				int a = m_VertexWeights[iIndex].Insert(boneIndex, fWeight);
			}

			// BONE 좌표계 변환 행렬 구하기
			FbxAMatrix matXBindPosLink;
			FbxAMatrix matReferenceGlobalInitPosition;
			cluster->GetTransformLinkMatrix(matXBindPosLink);
			cluster->GetTransformMatrix(matReferenceGlobalInitPosition);
			FbxAMatrix matWorldBindPose = matReferenceGlobalInitPosition.Inverse() * matXBindPosLink;
			FbxAMatrix matBindPose = matWorldBindPose.Inverse();
			Matrix mat = DxConvertMatrix(ConvertAMatrix(matBindPose));

			inverseBindPose[boneIndex] = mat;

		}
	}
	//if (!bRet)
	//{
	//	vector<Matrix> tempMatBone;
	//	// 시도 1 : 단위행렬
	//	{
	//		//tempMatBone.resize(m_FbxNodes.size());
	//		//_result->_inverseMatBone[_result->_inverseMatBone.size() - 1] = tempMatBone;

	//	}

	//	// 시도 2 : 바인드포즈 재구성
	//	{
	//		FbxAMatrix meshTransform = mesh->GetNode()->EvaluateGlobalTransform();
	//		FbxAMatrix correctionMat = meshTransform.Inverse();

	//		Matrix tempMat = DxConvertMatrix(ConvertAMatrix(correctionMat));
	//		for (int i = 0; i < m_FbxNodes.size(); i++)
	//		{
	//			tempMatBone.emplace_back(tempMat);
	//		}

	//		_result->_inverseMatBone[_result->_inverseMatBone.size() - 1] = tempMatBone;
	//	}
	//}
	m_result.m_vInverseBindPose.emplace_back(inverseBindPose);
	return bRet;
}

void FbxLoader::ParseAnimation()
{
	// 애니메이션 시 원점에 위치시키기 위해
	auto iter = m_FbxBones.find(L"Armature");
	if (iter != m_FbxBones.end())
	{
		FbxVector4 rootP = iter->second.m_node->EvaluateGlobalTransform(0).GetT();

		m_vRootPos.x = -rootP[0];
		m_vRootPos.y = -rootP[1];
		m_vRootPos.z = -rootP[2];
	}

	// ANIM
	int animBoneCount = m_result.m_mSkeletonList.size() + m_result.m_vMeshList.size();
	int animTrackCount = m_pScene->GetSrcObjectCount<FbxAnimStack>();
	m_result.m_iAnimTrackCount = animTrackCount;

	// TEMP FOR crow_final ( 애니메이션 개많음. 추려서 파싱 ) 
	{
		//vector<int> targetAnimList = { 50, 29, 5, 44, 45, 46, 47, 0, 15, 23, 25};
		//int size = targetAnimList.size();
		//if (m_result.m_iAnimTrackCount > 50)
		//{
		//	m_result.m_iAnimTrackCount = size;
		//}

		//for (int i = 0; i < size; i++)
		//{
		//	GetAnimationTrack(targetAnimList[i], animBoneCount);
		//}
	}

	for (int iTrack = 0; iTrack < m_result.m_iAnimTrackCount; iTrack++)
	{
		GetAnimationTrack(iTrack, animBoneCount);
	}
}

void FbxLoader::GetAnimationTrack(int animTrack, int nodeCount)
{
	repeatCount = 0;
	lastMeaningfulFrame = -1;
	maxValidFrame = 0;

	AnimTrackData track;
	track.m_vAnim.resize(nodeCount);
	GetAnimation(animTrack, &track);
	for (int iBone = 0; iBone < m_result.m_iBoneCount; iBone++)
	{
		auto iter = m_FbxBones.find(m_result.m_mSkeletonList[iBone].m_szName);
		if (iter == m_FbxBones.end())
		{
			continue;
		}
		track.m_vAnim[iBone] = GetNodeAnimation(iter->second.m_node, &track);
	}
	for (int iMesh = 0; iMesh < m_result.m_iMeshCount; iMesh++)
	{
		track.m_vAnim[m_result.m_iBoneCount + iMesh] = GetNodeAnimation(m_vMeshes[iMesh], &track);
	}

	if (maxValidFrame > 0)
	{
		for (int i = 0; i < m_result.m_iNodeCount; i++)
		{
			track.m_vAnim[i].resize(maxValidFrame);
		}
	}

	m_result.m_vAnimTrackList.emplace_back(track);
}

void FbxLoader::GetAnimation(int _animTrack, AnimTrackData* _track)
{
	FbxTime::SetGlobalTimeMode(FbxTime::eFrames30);
	FbxAnimStack* stack = m_pScene->GetSrcObject<FbxAnimStack>(_animTrack);
	_track->m_szName = to_mw(stack->GetName());

	if (stack == nullptr) nullptr;
	m_pScene->SetCurrentAnimationStack(stack);

	FbxString TakeName = stack->GetName();
	FbxTakeInfo* TakeInfo = m_pScene->GetTakeInfo(TakeName);
	FbxTimeSpan LocalTimeSpan = TakeInfo->mLocalTimeSpan;
	FbxTime start = LocalTimeSpan.GetStart();
	FbxTime end = LocalTimeSpan.GetStop();
	FbxTime Duration = LocalTimeSpan.GetDuration();

	FbxTime::EMode TimeMode = FbxTime::GetGlobalTimeMode();
	FbxLongLong s = start.GetFrameCount(TimeMode);
	FbxLongLong n = end.GetFrameCount(TimeMode);
	float endTime = static_cast<float>(TakeInfo->mLocalTimeSpan.GetStop().GetFrameCount());
	_track->m_iStartFrame = s;
	_track->m_iEndFrame = endTime;
}

vector<Matrix> FbxLoader::GetNodeAnimation(FbxNode* _node, AnimTrackData* _track)
{
	vector<Matrix> ret;
	Matrix prevMat;

	for (FbxLongLong t = _track->m_iStartFrame; t <= _track->m_iEndFrame; t++)
	{
		FbxTime time;
		FbxTime::EMode TimeMode = FbxTime::GetGlobalTimeMode();
		time.SetFrame(t, TimeMode);
		FbxAMatrix matGlobal = _node->EvaluateGlobalTransform(time);

		// 원점으로 이동시키기 위함
		FbxAMatrix correctionR;
		correctionR.SetIdentity();
		correctionR.SetR(FbxVector4(90.0f, 0.0f, 0.0f));
		FbxAMatrix correctionT;
		correctionT.SetIdentity();
		correctionT.SetT(FbxVector4(m_vRootPos.x, m_vRootPos.y, m_vRootPos.z));

		FbxAMatrix finalMatrix = correctionR * correctionT * matGlobal;
		Matrix mat = DxConvertMatrix(ConvertAMatrix(finalMatrix));

		if (t != _track->m_iStartFrame && prevMat == mat) {
			repeatCount++;
		}
		else {
			repeatCount = 0;
		}

		if (repeatCount < repeatThreshold) {
			lastMeaningfulFrame = t;
		}
		maxValidFrame = maxValidFrame < lastMeaningfulFrame ? lastMeaningfulFrame : maxValidFrame;


		prevMat = mat;
		ret.emplace_back(mat);
	}
	return ret;
}

string FbxLoader::ParseMaterial(FbxSurfaceMaterial* _pSurface)
{
	const char* scTexName;
	auto property = _pSurface->FindProperty(FbxSurfaceMaterial::sDiffuse);
	if (property.IsValid()) // 유효성 체크
	{
		FbxFileTexture* texFile = property.GetSrcObject<FbxFileTexture>(0);
		if (texFile)
		{
			scTexName = texFile->GetFileName();

			// SPLIT TEXNAME
			CHAR Drive[MAX_PATH];
			CHAR Dir[MAX_PATH];
			CHAR FName[MAX_PATH];
			CHAR Ext[MAX_PATH];

			_splitpath_s(scTexName, Drive, Dir, FName, Ext);
			string texName = FName;
			string ext = Ext;

			if (ext == ".tga" || ext == ".TGA")
			{
				ext.clear();
				ext = ".dds";
			}
			texName += ext;
			return texName;
		}
	}

	return string();
}

FbxColor FbxLoader::ReadColor(FbxMesh* mesh, DWORD dwVertexColorCount, FbxLayerElementVertexColor* pVertexColorSet, DWORD dwDCCIndex, DWORD dwVertexIndex)
{
	FbxColor Value(1, 1, 1, 1);
	if (dwVertexColorCount > 0 && pVertexColorSet != NULL)
	{
		switch (pVertexColorSet->GetMappingMode())
		{
		case FbxLayerElement::eByControlPoint:
		{
			switch (pVertexColorSet->GetReferenceMode())
			{
			case FbxLayerElement::eDirect:
			{
				Value = pVertexColorSet->GetDirectArray().GetAt(dwDCCIndex);
			}break;
			case FbxLayerElement::eIndexToDirect:
			{
				int iColorIndex = pVertexColorSet->GetIndexArray().GetAt(dwDCCIndex);
				Value = pVertexColorSet->GetDirectArray().GetAt(iColorIndex);
			}break;
			}
		}
		case FbxLayerElement::eByPolygonVertex:
		{
			switch (pVertexColorSet->GetReferenceMode())
			{
			case FbxLayerElement::eDirect:
			{
				int iColorIndex = dwVertexIndex;
				Value = pVertexColorSet->GetDirectArray().GetAt(iColorIndex);
			}break;
			case FbxLayerElement::eIndexToDirect:
			{
				int iColorIndex = pVertexColorSet->GetIndexArray().GetAt(dwVertexIndex);
				Value = pVertexColorSet->GetDirectArray().GetAt(iColorIndex);
			}break;
			}
		}
		}
	}
	return Value;
}

FbxVector4 FbxLoader::ReadNormal(const FbxMesh* mesh, DWORD dwVertexNormalCount, FbxLayerElementNormal* VertexNormalSets, int controlPointIndex, int iVertexIndex)
{
	FbxVector4 result(0, 0, 0);
	if (dwVertexNormalCount < 1)
	{
		return result;
	}
	int iVertexNormalLayder = mesh->GetElementNormalCount();

	const FbxGeometryElementNormal* vertexNormal = mesh->GetElementNormal(0);

	switch (VertexNormalSets->GetMappingMode())
	{
	case FbxGeometryElement::eByControlPoint:
	{
		// control point mapping 
		switch (VertexNormalSets->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
		{
			result[0] = static_cast<float>(VertexNormalSets->GetDirectArray().GetAt(controlPointIndex).mData[0]);
			result[1] = static_cast<float>(VertexNormalSets->GetDirectArray().GetAt(controlPointIndex).mData[1]);
			result[2] = static_cast<float>(VertexNormalSets->GetDirectArray().GetAt(controlPointIndex).mData[2]);
		} break;
		case FbxGeometryElement::eIndexToDirect:
		{
			int index = VertexNormalSets->GetIndexArray().GetAt(controlPointIndex);
			// 인덱스를 얻어온다. 
			result[0] = static_cast<float>(VertexNormalSets->GetDirectArray().GetAt(index).mData[0]);
			result[1] = static_cast<float>(VertexNormalSets->GetDirectArray().GetAt(index).mData[1]);
			result[2] = static_cast<float>(VertexNormalSets->GetDirectArray().GetAt(index).mData[2]);
		}break;
		}break;
	}break;
	// 정점 마다 1개의 매핑 좌표가 있다.
	case FbxGeometryElement::eByPolygonVertex:
	{
		switch (vertexNormal->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
		{
			result[0] = static_cast<float>(VertexNormalSets->GetDirectArray().GetAt(iVertexIndex).mData[0]);
			result[1] = static_cast<float>(VertexNormalSets->GetDirectArray().GetAt(iVertexIndex).mData[1]);
			result[2] = static_cast<float>(VertexNormalSets->GetDirectArray().GetAt(iVertexIndex).mData[2]);
		}
		break;
		case FbxGeometryElement::eIndexToDirect:
		{
			int index = VertexNormalSets->GetIndexArray().GetAt(iVertexIndex);
			// 인덱스를 얻어온다. 
			result[0] = static_cast<float>(VertexNormalSets->GetDirectArray().GetAt(index).mData[0]);
			result[1] = static_cast<float>(VertexNormalSets->GetDirectArray().GetAt(index).mData[1]);
			result[2] = static_cast<float>(VertexNormalSets->GetDirectArray().GetAt(index).mData[2]);
		}break;
		}
	}break;
	}
	return result;
}

void FbxLoader::ReadTextureCoord(FbxMesh* pFbxMesh, FbxLayerElementUV* pUVSet, int vertexIndex, int uvIndex, FbxVector2& uv)
{
	FbxLayerElementUV* pFbxLayerElementUV = pUVSet;
	if (pFbxLayerElementUV == nullptr) {
		return;
	}

	switch (pFbxLayerElementUV->GetMappingMode())
	{
	case FbxLayerElementUV::eByControlPoint:
	{
		switch (pFbxLayerElementUV->GetReferenceMode())
		{
		case FbxLayerElementUV::eDirect:
		{
			FbxVector2 fbxUv = pFbxLayerElementUV->GetDirectArray().GetAt(vertexIndex);
			uv.mData[0] = fbxUv.mData[0];
			uv.mData[1] = fbxUv.mData[1];
			break;
		}
		case FbxLayerElementUV::eIndexToDirect:
		{
			int id = pFbxLayerElementUV->GetIndexArray().GetAt(vertexIndex);
			FbxVector2 fbxUv = pFbxLayerElementUV->GetDirectArray().GetAt(id);
			uv.mData[0] = fbxUv.mData[0];
			uv.mData[1] = fbxUv.mData[1];
			break;
		}
		}
		break;
	}
	case FbxLayerElementUV::eByPolygonVertex:
	{
		switch (pFbxLayerElementUV->GetReferenceMode())
		{
		case FbxLayerElementUV::eDirect: // fbx 5.0 이하일 때
		case FbxLayerElementUV::eIndexToDirect:
		{
			uv.mData[0] = pFbxLayerElementUV->GetDirectArray().GetAt(uvIndex).mData[0];
			uv.mData[1] = pFbxLayerElementUV->GetDirectArray().GetAt(uvIndex).mData[1];
			break;
		}
		}
		break;
	}
	}
}

int FbxLoader::GetSubMaterialIndex(int iPoly, FbxLayerElementMaterial* pMaterialSetList)
{
	int iSubMtrl = 0;
	if (pMaterialSetList != nullptr)
	{
		switch (pMaterialSetList->GetMappingMode())
		{
		case FbxLayerElement::eByPolygon:
		{
			// 매핑 정보가 배열에 저장되는 방식
			switch (pMaterialSetList->GetReferenceMode())
			{
			case FbxLayerElement::eIndex:
			{
				iSubMtrl = iPoly;
			}break;
			case FbxLayerElement::eIndexToDirect:
			{
				iSubMtrl = pMaterialSetList->GetIndexArray().GetAt(iPoly);
			}break;
			}
		}
		default:
		{
			break;
		}
		}
	}
	return iSubMtrl;
}

Matrix FbxLoader::DxConvertMatrix(Matrix m)
{
	Matrix mat;
	mat._11 = m._11; mat._12 = m._13; mat._13 = m._12;
	mat._21 = m._31; mat._22 = m._33; mat._23 = m._32;
	mat._31 = m._21; mat._32 = m._23; mat._33 = m._22;
	mat._41 = m._41; mat._42 = m._43; mat._43 = m._42;
	mat._14 = mat._24 = mat._34 = 0.0f;
	mat._44 = 1.0f;
	return mat;
}
Matrix FbxLoader::ConvertAMatrix(FbxAMatrix& m)
{
	Matrix mat;
	float* pMatArray = reinterpret_cast<float*>(&mat);
	double* pSrcArray = reinterpret_cast<double*>(&m);
	for (int i = 0; i < 16; i++)
	{
		pMatArray[i] = pSrcArray[i];
	}
	return mat;
}