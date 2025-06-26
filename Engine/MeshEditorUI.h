#pragma once
#include <functional>

class UMeshComponent;
class UMeshResources;
class UAnimInstance;
class APawn;

struct PreMeshData
{
    // Base
    bool bRoot = false;
    bool bVisible = false;
    bool bSkeletal = false;
    wstring meshName = L"";
    wstring compName = L"";
    wstring parentCompName = L"";
    // Mat
    int texIndex = 0;
    string texPath = "";
    string shaderPath = "";
    Vec3 specular = {0,0,0};
    float shininess = 0.f;
    // Anim
    string parentBoneName = "";
    string rootBoneName = "";
    int animIndex = 0;
    bool bAnimatedStatic = false;
    bool bRootMotion = false;
    bool bInverseMatBone = false;
    // WILL BE DEPRECATED
    shared_ptr<APawn> actor = nullptr;
};

class MeshEditorUI
{
public:
    void DrawUI();

    
    // 생성
    void CreateActorCallback(function<void(PreMeshData, shared_ptr<APawn>&)> callback)
    {
        m_OnCreateActor = std::move(callback);
    }
    void CreateChildMeshCallback(function<void(PreMeshData)> callback)
    {
        m_OnCreateChild = std::move(callback);
    }

    // mesh 변경
    void MoveMesh(std::function<void(string, Vec3)> callback)
    {
        m_OnMoveChild = std::move(callback);
    }
    void ScaleMesh(std::function<void(string, Vec3)> callback)
    {
        m_OnScaleChild = std::move(callback);
    }
    void RotateMesh(std::function<void(string, Vec3)> callback)
    {
        m_OnRotateChild = std::move(callback);
    }

    // animation 관련
    void StopAnim(std::function<void(bool)> callback)
    {
        m_OnAnimStop = std::move(callback);
    }
    void ChangeAhim(std::function<void(int)> callback)
    {
        m_OnAnimChange = std::move(callback);
    }
    void ChangeParentBone(std::function<void(string, string, bool)> callback)
    {
        m_OnParentBoneChange = std::move(callback);
    }
    void ChangeVisible(std::function<void(string, bool)> callback)
    {
        m_OnChangeMeshVisible = std::move(callback);
    }
    void RemoveMesh(std::function<void(string)> callback)
    {
        m_OnRemoveChild = std::move(callback);
    }
    // 저장
    void SaveMesh(std::function<void(int, bool, string)> callback)
    {
        m_OnMeshSave = std::move(callback);
    }

public:
    void DrawMatUI();
    void DrawMeshHierarchy();
    void DrawMeshRecursive(shared_ptr<UMeshComponent> _child);
    
    void DrawSetRoot();
    void DrawRemoveChild();
    void DrawAddChild();
    void DrawModifyMesh();
    void DrawAnimFactor();
    void DrawSave();

public:
    void SetMeshList(map<wstring, shared_ptr<UMeshResources>> _meshList);
    void SetAnimList(vector<shared_ptr<UAnimInstance>> _animList);
    void SetTexList(vector<wstring> _texureList);
    void SetBoneList(wstring _meshName);
    void SetActor(shared_ptr<APawn>& _actor);
public:
    void UpdateChildComboData();
    void UpdateChildRecursive(shared_ptr<UMeshComponent> _comp);

private:
    map<wstring, shared_ptr<UMeshResources>> m_mMeshResMap;
    vector<string> m_vMeshNameList;
    vector<const char*> m_vMeshPtrList;

    vector<string> m_vChildCompNameList;
    vector<const char*> m_vChildCompPtrList;

    vector<string> m_vAnimList;
    vector<const char*> m_vAnimPtrList;
    
    vector<string> m_vAnimTrackList;
    vector<const char*> m_vAnimTrackPtrList;

    vector<string> m_vTexList;
    vector<const char*> m_vTexPtrList;
    char texPath[250] = { 0, };
    char shaderPath[250] = { 0, };
    char compName[250] = { 0, };
    Vec3 m_specular = Vec3();
    float m_shininess = 0.f;

    vector<string> m_vBoneList;
    vector<const char*> m_vBonePtrList;

    bool m_bBoneSetting = false;

    int m_meshIndex = 0;
    int m_animIndex = 0;
    int m_texIndex = 0;
    int m_parentBoneIndex = -1;
    int m_rootBoneIndex = -1;

    bool m_modifyChild = false;
    bool m_inverseMatBone = false;
    bool m_moveChildMesh = false;
    int m_childIndex = 0;
    Vec3 m_pos = Vec3();
    Vec3 m_scale = Vec3();
    Vec3 m_rot = Vec3();
    PreMeshData m_childData = PreMeshData();

    bool m_animStop = false;        // 초기화 추가
    int m_currentAnim = 0;          // 초기화 추가
    bool m_bActorSet = false;       // 초기화 추가

    /// <summary>
    /// flag 정리
    /// </summary>
    bool m_bRootCreated = false;    // 초기화 추가
    bool m_bSkinnedRoot = false;    // 초기화 추가
    char m_meshSaveName[250];
    bool m_bVisible = true;
    bool m_bAnimatedStatic = false;


    shared_ptr<APawn> m_pActor = nullptr;
    string m_selectedMeshName = "";
    string m_rootMeshName = "";

    function<void(PreMeshData, shared_ptr<APawn>&)> m_OnCreateActor = nullptr;
    function<void(PreMeshData)> m_OnCreateChild = nullptr;
    function<void(string, Vec3)> m_OnMoveChild = nullptr;
    function<void(string, Vec3)> m_OnScaleChild = nullptr;
    function<void(string, Vec3)> m_OnRotateChild = nullptr;
    function<void(bool)> m_OnAnimStop = nullptr;
    function<void(int)> m_OnAnimChange = nullptr;
    function<void(string, string, bool )> m_OnParentBoneChange = nullptr;
    function<void(int, bool, string)> m_OnMeshSave = nullptr;
    function<void(string, bool)> m_OnChangeMeshVisible = nullptr;
    function<void(string)> m_OnRemoveChild = nullptr;


};

