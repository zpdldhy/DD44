#pragma once
#include "SkeletalMeshData.h"

class APawn;

class AssimpLoader
{
private:
    const aiScene* m_pScene;        //�� ����
    vector<MeshData> m_meshes;        //�Ž� ����
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
    //void Destroy();
};

