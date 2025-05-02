#pragma once

class MapEditorUI
{
public:
    void Update();

    int GetNumCols() const { return m_iCols; }
    int GetNumRows() const { return m_iRows; }
    float GetCellSize() const { return m_fCellSize; }

    Vec3 GetPosition() const { return Vec3(m_fPosition[0], m_fPosition[1], m_fPosition[2]); }
    Vec3 GetRotation() const { return Vec3(m_fRotation[0], m_fRotation[1], m_fRotation[2]); }
    Vec3 GetScale() const { return Vec3(m_fScale[0], m_fScale[1], m_fScale[2]); }

    wstring GetTexturePath() const { return wstring(m_szTexturePath, m_szTexturePath + strlen(m_szTexturePath)); }
    wstring GetShaderPath() const { return wstring(m_szShaderPath, m_szShaderPath + strlen(m_szShaderPath)); }


private:
    int m_iCols = 20;
    int m_iRows = 20;
    float m_fCellSize = 10.0f;

    float m_fPosition[3] = { 0.0f, 0.0f, 0.0f };
    float m_fRotation[3] = { 0.0f, 0.0f, 0.0f };
    float m_fScale[3] = { 1.0f, 1.0f, 1.0f };

    char m_szTexturePath[256] = "../Resources/Texture/grass.jpg";
    char m_szShaderPath[256] = "../Resources/Shader/Default.hlsl";
};

