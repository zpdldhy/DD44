#pragma once
#include "FbxLoader.h"
#include "ActorLoader.h"
#include "PrefabLoader.h"
#include "UMeshComponent.h"
#include "MeshLoader.h"

class CharacterEditorUI
{
public:
    void DrawUI();
    void DrawVec3Slider(const char* label, float* values, float minVal, float maxVal);

    void SetOnCreateCallback(std::function<void(
        std::shared_ptr<UMeshComponent> rootComponent,
        Vec3 position,
        Vec3 rotation,
        Vec3 scale,
        int scriptType
        )> callback)
    {
        m_OnCreate = std::move(callback);
    }
//public:
//    shared_ptr<UMeshComponent> MakeMesh(MeshComponentData data, bool bRoot, shared_ptr<UAnimInstance> animInstance);
private:
    char m_szAssetPath[256] = "../Resources/Asset/crow_final.asset";
    char m_szMeshPath[256] = "../Resources/Asset/crow_final.mesh";

    char m_szTextureName[256] = "../Resources/Texture/Crow_DIFF.png";
    char m_szShaderName[256] = "../Resources/Shader/skinningShader.hlsl";

    char m_szChildTextureName[256] = "../Resources/Texture/sword.png";
    char m_szChildShaderName[256] = "../Resources/Shader/Default.hlsl";
    
    int m_iSelectedMeshIndex = 0;
    int m_iSelectedChildMeshIndex = 0;
    int m_iSelectedAnimIndex = 0;
    int m_iSelectedBoneIndex = 0;
    int m_iSelectedScriptIndex = 0;

    float m_fPosition[3] = { 0.0f, 0.0f, 0.0f };
    float m_fRotation[3] = { 0.0f, 0.0f, 0.0f };
    float m_fScale[3] = { 1.0f, 1.0f, 1.0f };

    std::shared_ptr<ActorLoader> m_pLoader;
    std::vector<std::shared_ptr<UMeshComponent>> m_vMeshList;
    std::map<wstring, std::shared_ptr<UMeshResources>> m_mMeshMap;
    std::vector<MeshComponentData> m_vMeshDataList;
    std::shared_ptr<UMeshComponent> m_pRootComponent;

    // Prefab
    std::vector<std::string> m_vCharacterPrefabList;

    std::function<void(
        std::shared_ptr<UMeshComponent> rootComponent,
        Vec3 position,
        Vec3 rotation,
        Vec3 scale,
        int scriptType
    )> m_OnCreate;

public:
    static bool m_bRootSet;
    static bool m_bChildAttached;
    static int m_iChildIndex;

    vector<shared_ptr<UAnimInstance>> m_vAnimList;
    map<wstring, shared_ptr<UAnimInstance>> m_mAnimMap;

    MeshLoader meshLoader;
};
