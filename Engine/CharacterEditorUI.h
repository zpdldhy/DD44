#pragma once
#include "FbxLoader.h"

class CharacterEditorUI
{
public:
    void DrawUI();
    void DrawVec3Slider(const char* label, float* values, float minVal, float maxVal);

    void SetOnCreateCallback(std::function<void(
        int actorType,
        int componentType,
        const char* assetPath,
        const Vec3& position,
        const Vec3& rotation,
        const Vec3& scale,
        int scriptType
        )> callback)
    {
        m_OnCreate = std::move(callback);
    }


private:
    int m_iActorType = 0;       // 0: Character, 1: Enemy
    int m_iComponentType = 0;   // 0: Skinned, 1: Static
    int m_iScriptType = 0;      // 0 = None, 1 = PlayerMoveScript, 2 = EnemyAIScript
    char m_szAssetPath[256] = "../Resources/Asset/crow_final.asset";

    float m_fPosition[3] = { 0.0f, 0.0f, 0.0f };
    float m_fRotation[3] = { 0.0f, 0.0f, 0.0f };
    float m_fScale[3] = { 1.0f, 1.0f, 1.0f };

    std::function<void(
        int actorType,
        int componentType,
        const char* assetPath,
        const Vec3& position,
        const Vec3& rotation,
        const Vec3& scale,
        int scriptType
        )> m_OnCreate;
};
