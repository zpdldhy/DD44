#pragma once
#include <functional>

class UMeshResources;
class UAnimInstance;
class APawn;

struct PreMeshData
{
    // Base
    bool bRoot;
    bool bSkeletal;
    wstring meshName;
    // Mat
    int texIndex;
    string texPath;
    string shaderPath;
    Vec3 specular = {0,0,0};
    float shininess;
    // Anim
    string parentBoneName;
    string rootBoneName;
    int animIndex;
    bool bAnimatedStatic;
    bool bRootMotion = false;
    bool bInverseMatBone = false;
    // WILL BE DEPRECATED
    shared_ptr<APawn> actor;
};

class MeshEditorUI
{
public:
    void DrawUI();

    
    // 积己
    void CreateActorCallback(function<void(PreMeshData, shared_ptr<APawn>&)> callback)
    {
        m_OnCreateActor = std::move(callback);
    }
    void CreateChildMeshCallback(function<void(PreMeshData, shared_ptr<APawn>&)> callback)
    {
        m_OnCreateChild = std::move(callback);
    }

    // mesh 函版
    void MoveMesh(std::function<void(int, Vec3)> callback)
    {
        m_OnMoveChild = std::move(callback);
    }

    // animation 包访
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

    // 历厘
    void SaveMesh(std::function<void(int, bool, string)> callback)
    {
        m_OnMeshSave = std::move(callback);
    }
public:
    void DrawMatUI();
public:
    void SetMeshList(map<wstring, shared_ptr<UMeshResources>> _meshList);
    void SetAnimList(vector<shared_ptr<UAnimInstance>> _animList);
    void SetTexList(vector<wstring> _texureList);
    void SetBoneList(wstring _meshName);
    void SetActor(shared_ptr<APawn>& _actor);

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
    char shaderPath[250];
    Vec3 m_specular;
    float m_shininess;

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
    bool m_inverseMatBone = false;
    bool m_moveChildMesh = false;
    int m_childIndex;
    Vec3 m_pos;
    PreMeshData m_childData;

    bool m_animStop;
    int m_currentAnim;
    bool m_bActorSet;


    /// <summary>
    /// flag 沥府
    /// </summary>
    bool m_bRootCreated;
    bool m_bSkinnedRoot;
    char m_meshSaveName[250];


    shared_ptr<APawn> m_pActor;

    //function<void(wstring, int, int, int, shared_ptr<APawn>&, wstring)> m_OnCreateActor;
    function<void(PreMeshData, shared_ptr<APawn>&)> m_OnCreateActor;
    function<void(PreMeshData, shared_ptr<APawn>&)> m_OnCreateChild;
    function<void(int, Vec3)> m_OnMoveChild;
    function<void(bool)> m_OnAnimStop;
    function<void(int)> m_OnAnimChange;
    function<void(int, string, bool )> m_OnParentBoneChange;
    function<void(int, bool, string)> m_OnMeshSave;

};

