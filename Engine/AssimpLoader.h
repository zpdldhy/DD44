#pragma once
#include "SkeletalMeshData.h"

class AssimpLoader
{
private:
    Assimp::Importer m_importer;
    const aiScene* m_pScene = nullptr;      // Assimp::Importer ���� �޸� ����
    vector<MeshData> m_meshes;    // �Ž� ����
    UINT m_numVertices = 0;
    UINT m_numMaterial = 0;
    Vec3 center = Vec3();
public:

    vector<MeshData> Load(const string& fileName);
    bool Init(const string& fileName);
    void InitScene();
    void ParseMesh(UINT index, const aiMesh* pMesh);
    UINT getNumMesh() const { return (UINT)m_meshes.size(); }
    void GetCenter(const aiMesh* pMesh);
};

