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
	m_pScene = aiImportFile(fileName.c_str(), aiProcess_JoinIdenticalVertices |        // ������ ������ ����, �ε��� ����ȭ
		aiProcess_ValidateDataStructure |        // �δ��� ����� ����
		aiProcess_ImproveCacheLocality |        // ��� ������ ĳ����ġ�� ����
		aiProcess_RemoveRedundantMaterials |    // �ߺ��� ���͸��� ����
		aiProcess_GenUVCoords |                    // ����, ������, ���� �� ��� ������ ������ UV�� ��ȯ
		aiProcess_TransformUVCoords |            // UV ��ȯ ó���� (�����ϸ�, ��ȯ...)
		aiProcess_FindInstances |                // �ν��Ͻ��� �Ž��� �˻��Ͽ� �ϳ��� �����Ϳ� ���� ������ ����
		aiProcess_LimitBoneWeights |            // ������ ���� ����ġ�� �ִ� 4���� ����
		aiProcess_OptimizeMeshes |                // ������ ��� ���� �Ž��� ����
		aiProcess_GenSmoothNormals |            // �ε巯�� �븻����(��������) ����
		aiProcess_SplitLargeMeshes |            // �Ŵ��� �ϳ��� �Ž��� �����Ž���� ��Ȱ(����)
		aiProcess_Triangulate |                    // 3�� �̻��� �𼭸��� ���� �ٰ��� ���� �ﰢ������ ����(����)
		aiProcess_ConvertToLeftHanded |            // D3D�� �޼���ǥ��� ��ȯ
		aiProcess_SortByPType);                    // ����Ÿ���� ������Ƽ��� ������ '������' �Ž��� ����

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

	//�ﰢ���̹Ƿ� ���� �̷�� ������ 3��
	
	// ������ ��ġ ��Ű��
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

	// ��� �޽��� ��� ���� ��ȸ
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
