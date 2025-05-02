#include "pch.h"
#include "AssimpLoader.h"

vector<MeshData> AssimpLoader::Load(const string& fileName)
{
	m_meshes.clear();

	if (!Init(fileName))
	{
		assert(false);
	}

	InitScene();

	return m_meshes;
}

bool AssimpLoader::Init(const string& fileName)
{
	m_pScene = aiImportFile(fileName.c_str(), aiProcess_JoinIdenticalVertices |        // 동일한 꼭지점 결합, 인덱싱 최적화
		aiProcess_ValidateDataStructure |        // 로더의 출력을 검증
		aiProcess_ImproveCacheLocality |        // 출력 정점의 캐쉬위치를 개선
		aiProcess_RemoveRedundantMaterials |    // 중복된 매터리얼 제거
		aiProcess_GenUVCoords |                    // 구형, 원통형, 상자 및 평면 매핑을 적절한 UV로 변환
		aiProcess_TransformUVCoords |            // UV 변환 처리기 (스케일링, 변환...)
		aiProcess_FindInstances |                // 인스턴스된 매쉬를 검색하여 하나의 마스터에 대한 참조로 제거
		aiProcess_LimitBoneWeights |            // 정점당 뼈의 가중치를 최대 4개로 제한
		aiProcess_OptimizeMeshes |                // 가능한 경우 작은 매쉬를 조인
		aiProcess_GenSmoothNormals |            // 부드러운 노말벡터(법선벡터) 생성
		aiProcess_SplitLargeMeshes |            // 거대한 하나의 매쉬를 하위매쉬들로 분활(나눔)
		aiProcess_Triangulate |                    // 3개 이상의 모서리를 가진 다각형 면을 삼각형으로 만듬(나눔)
		aiProcess_ConvertToLeftHanded |            // D3D의 왼손좌표계로 변환
		aiProcess_SortByPType);                    // 단일타입의 프리미티브로 구성된 '깨끗한' 매쉬를 만듬

	if (m_pScene)
	{
		m_meshes.resize(m_pScene->mNumMeshes);
		m_numMaterial = m_pScene->mNumMaterials;
		return true;
	}
	else
	{
		return false;
	}
}

void AssimpLoader::InitScene()
{
	for (UINT i = 0; i < m_meshes.size(); ++i)
	{
		const aiMesh* pMesh = m_pScene->mMeshes[i];
		ParseMesh(i, pMesh);
		m_numVertices += (UINT)m_meshes[i].m_vVertexList.size();
	}
}


void AssimpLoader::ParseMesh(UINT index, const aiMesh* pMesh)
{
	int matNum = m_pScene->mNumMaterials;
	aiMaterial* material = m_pScene->mMaterials[index];

	if (material->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
		aiString str;
		material->GetTexture(aiTextureType_DIFFUSE, 0, &str);
	}


	m_meshes[index].m_vVertexList.reserve(pMesh->mNumVertices);
	m_meshes[index].m_vIndexList.reserve(pMesh->mNumFaces * 3);

	//삼각형이므로 면을 이루는 꼭짓점 3개
	
	// 원점에 위치 시키기
	GetCenter(pMesh);

	for (UINT i = 0; i < pMesh->mNumVertices; ++i)
	{
		Vec3 pos = Vec3(pMesh->mVertices[i].x, pMesh->mVertices[i].y, pMesh->mVertices[i].z);
		Vec3 normal = Vec3(pMesh->mNormals[i].x, pMesh->mNormals[i].y, pMesh->mNormals[i].z);
		Vec2 uv;
		Vec4 color = { 0.0f, 0.0f, 0.0f, 1.0f };
		if (pMesh->HasTextureCoords(0))
		{
			uv = Vec2(pMesh->mTextureCoords[0][i].x, pMesh->mTextureCoords[0][i].y);
		}
		else
		{
			uv = Vec2(0.0f, 0.0f);
		}

		PNCT_VERTEX p;
		p.pos = pos - center;
		p.normal = normal;
		p.uv = uv;
		p.color = color;

		m_meshes[index].m_vVertexList.emplace_back(p);
	}


	for (UINT i = 0; i < pMesh->mNumFaces; ++i) {

		const aiFace& face = pMesh->mFaces[i];

		m_meshes[index].m_vIndexList.push_back(face.mIndices[0]);
		m_meshes[index].m_vIndexList.push_back(face.mIndices[1]);
		m_meshes[index].m_vIndexList.push_back(face.mIndices[2]);

	}
}

void AssimpLoader::GetCenter(const aiMesh* _pMesh)
{
	Vec3 minPos(FLT_MAX, FLT_MAX, FLT_MAX);
	Vec3 maxPos(-FLT_MAX, -FLT_MAX, -FLT_MAX);

	// 모든 메시의 모든 정점 순회
	for (unsigned int j = 0; j < _pMesh->mNumVertices; ++j) {
		aiVector3D v = _pMesh->mVertices[j];
		minPos.x = std::min(minPos.x, v.x);
		minPos.y = std::min(minPos.y, v.y);
		minPos.z = std::min(minPos.z, v.z);
		maxPos.x = max(maxPos.x, v.x);
		maxPos.y = max(maxPos.y, v.y);
		maxPos.z = max(maxPos.z, v.z);
	}
	center = (minPos + maxPos) * 0.5f;
}
