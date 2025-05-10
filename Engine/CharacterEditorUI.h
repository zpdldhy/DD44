#pragma once
#include "FbxLoader.h"
#include "ActorLoader.h"
#include "PrefabLoader.h"

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

private:
    char m_szTextureName[256] = "../Resources/Texture/kkongchi.jpg";
    char m_szShaderName[256] = "../Resources/Shader/Default.hlsl";
    char m_szAssetPath[256] = "../Resources/Asset/crow_final.asset";

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
    std::shared_ptr<UMeshComponent> m_pRootComponent;

    // Prefab
    std::vector<PrefabData> m_vPrefabList;

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
};
