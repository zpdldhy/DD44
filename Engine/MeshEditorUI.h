#pragma once
#include <functional>

class UMeshResources;
class UAnimInstance;
class APawn;

struct ChildMeshData
{
    wstring meshName;
    int texIndex;
    string texPath;
    string parentBoneName;
    string rootBoneName;
    bool bSkeletal;
    bool bAnimatedStatic;
    bool bRootMotion = false;
    bool bInverseMatBone = false;
    shared_ptr<APawn> rootMesh;
};

class MeshEditorUI
{
public:
    void DrawUI();
    void CreateActorCallback(std::function<void(wstring, int, int, int, shared_ptr<APawn>&)> callback)
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
    void StopAnim(std::function<void(bool)> callback)
    {
        m_OnAnimStop = std::move(callback);
    }
    void ChangeAhim(std::function<void(int)> callback)
    {
        m_OnAnimChange = std::move(callback);
    }
    void ChangeParentBone(std::function<void(int, string, bool)> callback)
    {
        m_OnParentBoneChange = std::move(callback);
    }
    void SaveMesh(std::function<void(int, bool)> callback)
    {
        m_OnMeshSave = std::move(callback);
    }
public:
    void SetMeshList(map<wstring, shared_ptr<UMeshResources>> _meshList);
    void SetAnimList(vector<shared_ptr<UAnimInstance>> _animList);
    void SetTexList(vector<wstring> _texureList);
    void SetBoneList(wstring _meshName);

private:
    map<wstring, shared_ptr<UMeshResources>> m_mMeshResMap;
    vector<string> m_vMeshNameList;
    vector<const char*> m_vMeshPtrList;
    vector<const char*> m_vChildMeshPtrList;

    vector<string> m_vAnimList;
    vector<const char*> m_vAnimPtrList;
    
    vector<string> m_vAnimTrackList;
    vector<const char*> m_vAnimTrackPtrList;

    vector<string> m_vTexList;
    vector<const char*> m_vTexPtrList;
    char texPath[250];

    vector<string> m_vBoneList;
    vector<const char*> m_vBonePtrList;

    bool m_bBoneSetting = false;

    int m_meshIndex;
    int m_animIndex;
    int m_texIndex;
    int m_parentBoneIndex = -1;
    int m_rootBoneIndex = -1;

    bool m_childMeshSelected = false;
    bool m_modifyChild = false;
    bool m_baseMeshSelected = false;
    bool m_inverseMatBone = false;
    bool m_moveChildMesh = false;
    int m_childIndex;
    Vec3 m_pos;
    ChildMeshData m_childData;

    bool m_animStop;
    int m_currentAnim;


    shared_ptr<APawn> m_pActor;

    function<void(wstring, int, int, int, shared_ptr<APawn>&)> m_OnCreateActor;
    function<void(ChildMeshData)> m_OnCreateChild;
    function<void(int, Vec3)> m_OnMoveChild;
    function<void(bool)> m_OnAnimStop;
    function<void(int)> m_OnAnimChange;
    function<void(int, string, bool )> m_OnParentBoneChange;
    function<void(int, bool)> m_OnMeshSave;

};

