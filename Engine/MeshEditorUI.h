#pragma once
#include <functional>

class UMeshResources;
class UAnimInstance;
class APawn;

struct ChildMeshData
{
    int meshIndex;
    int texIndex;
    string texPath;
    string parentBoneName;
    string rootBoneName;
    bool bSkeletal;
    bool bAnimatedStatic;
    bool bRootMotion = false;
    shared_ptr<APawn> rootMesh;
};

class MeshEditorUI
{
public:
    void DrawUI();
    void CreateActorCallback(std::function<void(int, int, int, shared_ptr<APawn>&)> callback)
    {
        m_OnCreateActor = std::move(callback);
    }
    void CreateChildMeshCallback(function<void(ChildMeshData)> callback)
    {
        m_OnCreateChild = std::move(callback);
    }
    void MoveMesh(std::function<void(int, Vec3)> callback)
    {
        m_OnMoveChild = std::move(callback);
    }
public:
    void SetMeshList(vector<shared_ptr<UMeshResources>> _meshList);
    void SetAnimList(vector<shared_ptr<UAnimInstance>> _animList);
    void SetTexList(vector<wstring> _texureList);
    void SetBoneList(int _meshIndex);

private:
    vector<shared_ptr<UMeshResources>> m_vMeshResList;
    vector<string> m_vMeshNameList;
    vector<const char*> m_vMeshPtrList;
    vector<const char*> m_vChildMeshPtrList;


    vector<string> m_vAnimList;
    vector<const char*> m_vAnimPtrList;
    
    vector<string> m_vTexList;
    vector<const char*> m_vTexPtrList;
    char texPath[250];

    vector<string> m_vBoneList;
    vector<const char*> m_vBonePtrList;

    int m_meshIndex;
    int m_animIndex;
    int m_texIndex;
    int m_parentBoneIndex = -1;
    int m_rootBoneIndex = -1;

    bool m_childMeshSelected = false;
    
    bool m_moveChildMesh = false;
    int m_childIndex;
    Vec3 m_pos;
    ChildMeshData m_childData;

    shared_ptr<APawn> m_pActor;

    function<void(int, int, int, shared_ptr<APawn>&)> m_OnCreateActor;
    function<void(ChildMeshData)> m_OnCreateChild;
    function<void(int, Vec3)> m_OnMoveChild;
};

