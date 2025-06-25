#pragma once
#include <functional>

class MapEditorUI
{
public:
    void DrawUI();

    void SetOnCreateCallback(std::function<void()> callback)
    {
        m_OnCreate = std::move(callback);
    }

    int GetNumCols() const { return m_iCols; }
    int GetNumRows() const { return m_iRows; }
    float GetCellSize() const { return m_fCellSize; }

    Vec3 GetPosition() const { return Vec3(m_fPosition[0], m_fPosition[1], m_fPosition[2]); }
    Vec3 GetRotation() const { return Vec3(m_fRotation[0], m_fRotation[1], m_fRotation[2]); }
    Vec3 GetScale() const { return Vec3(m_fScale[0], m_fScale[1], m_fScale[2]); }

    int GetSelectedRow() const { return m_SelectedRow; }
    int GetSelectedCol() const { return m_SelectedCol; }
    float GetTargetHeight() const { return m_TargetHeight; }

    std::wstring GetTexturePath() const;
    std::wstring GetShaderPath() const;

    bool IsSplattingMode() const { return m_bSplattingMode; }
    float GetBrushRadius() const { return m_fBrushRadius; }
    float GetBrushStrength() const { return m_fBrushStrength; }
    int GetBrushChannel() const { return m_iBrushChannel; }

private:
    int m_iCols = 20;
    int m_iRows = 20;
    float m_fCellSize = 10.0f;

    float m_fPosition[3] = { 0.0f, 0.0f, 0.0f };
    float m_fRotation[3] = { 0.0f, 0.0f, 0.0f };
    float m_fScale[3] = { 1.0f, 1.0f, 1.0f };

    int m_SelectedRow = 0;
    int m_SelectedCol = 0;
    float m_TargetHeight = 0.0f;

    char m_szTextureName[256] = "grass.jpg";
    char m_szShaderName[256] = "Default.hlsl";

    std::vector<std::string> m_vMapPrafabList;

    float m_fBrushRadius = 10.0f;
    float m_fBrushStrength = 0.3f;
    int   m_iBrushChannel = 0; // 0: R, 1: G, 2: B, 3: A

    bool  m_bSplattingMode = false;

    std::function<void()> m_OnCreate;
};
