#pragma once

class ObjectEditorUI
{
public:
    void DrawUI();
    void DrawVec3Slider(const char* label, float* values, float minVal, float maxVal);

    int GetActorIndexFromName(const std::string& name);
    std::vector<std::string> GetPrefabList(const std::string& folderPath);

    void SetOnCreateCallback(std::function<void(int, int, const char*, const char*, const char*, Vec3, Vec3, Vec3)> callback)
    {
        m_OnCreate = std::move(callback);
    }

private:
    int m_iCurrentActorType = 0;
    int m_iCurrentMeshType = 0;

    float m_fPosition[3] = { 0.0f, 0.0f, 0.0f };
    float m_fRotation[3] = { 0.0f, 0.0f, 0.0f };
    float m_fScale[3] = { 1.0f, 1.0f, 1.0f };

    char m_szTexturePath[256] = "../Resources/Texture/kkongchi.jpg";
    char m_szShaderPath[256] = "../Resources/Shader/Default.hlsl";
    char m_szObjPath[256] = "../Resources/Obj/buggy_floor.obj";

    std::string selectedPrefabName = "";
    std::vector<std::string> m_vPrefabList;

    std::function<void(int, int, const char*, const char*, const char*, Vec3, Vec3, Vec3)> m_OnCreate;
};
