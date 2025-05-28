#pragma once
#include "SkeletalMeshData.h"

class AssimpLoader
{
private:
    Assimp::Importer m_importer;
    const aiScene* m_pScene;      // Assimp::Importer 에서 메모리 관리
    vector<MeshData> m_meshes;    // 매쉬 정보
    UINT m_numVertices;
    UINT m_numMaterial;
    Vec3 center;
public:

    vector<MeshData> Load(const string& fileName);
    bool Init(const string& fileName);
    void InitScene();
    void ParseMesh(UINT index, const aiMesh* pMesh);
    UINT getNumMesh() const { return (UINT)m_meshes.size(); }
    void GetCenter(const aiMesh* pMesh);
};

