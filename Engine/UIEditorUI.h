#pragma once
class UIEditorUI
{
public:
    void DrawUI();
    void DrawVec3Slider(const char* label, float* values, float minVal, float maxVal);

    void SetOnCreateCallback(std::function<void(const char*, const char*, Vec3, Vec3, Vec3)> callback)
    {
        m_OnCreate = std::move(callback);
    }

private:
    float m_fPosition[3] = { 0.0f, 0.0f, 0.0f };
    float m_fRotation[3] = { 0.0f, 0.0f, 0.0f };
    float m_fScale[3] = { 100.0f, 100.0f, 1.0f };

    char m_szTexturePath[256] = "../Resources/Texture/white.png";
    char m_szShaderPath[256] = "../Resources/Shader/Default.hlsl";

    std::function<void(const char*, const char*, Vec3, Vec3, Vec3)> m_OnCreate;
};
