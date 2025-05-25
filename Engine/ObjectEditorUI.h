#pragma once
#include "EditorData.h"

class ObjectEditorUI
{
public:
    void DrawUI();
    void DrawVec3Slider(const char* label, float* values, float minVal, float maxVal);

    Vec3 SnapToGrid(const Vec3& position, float gridSize);
    bool IsSnapEnabled() const { return m_bSnapEnabled; }
    bool IsPlacementMode() const { return m_bPlacementMode; }

    std::vector<std::string> LoadMeshFileList(const std::string& folderPath);
    std::vector<std::string> LoadPrefabList(const std::string& folderPath);

    void StartFileBrowser(const std::string& root, const std::string& relative);
    void CreateAtPosition(const Vec3& pos);

    void SetOnCreateCallback(std::function<void(
        const char*, const char*, const char*, Vec3, Vec3, Vec3, Vec3, float, Vec3, float, ShapeComponentData)> callback)
    {
        m_OnCreate = std::move(callback);
    }

private:
    bool m_bSnapEnabled = false;
    bool m_bPlacementMode = false;

    int m_iSelectedPlacedIndex = -1;

    float m_fPosition[3] = { 0.0f, 0.0f, 0.0f };
    float m_fRotation[3] = { 0.0f, 0.0f, 0.0f };
    float m_fScale[3] = { 1.0f, 1.0f, 1.0f };

    char m_szObjPath[256] = "../Resources/Asset/stone_wall.asset";
    char m_szTexturePath[256] = "../Resources/Obj/wall/stone_wall.png";
    char m_szShaderPath[256] = "../Resources/Shader/Wall.hlsl";
 
    float m_fEmissiveColor[3] = { 0.f, 0.f, 0.f };
    float m_fEmissivePower = 0.f;
    float m_fSpecularColor[3] = { 0.f, 0.f, 0.f };
    float m_fShininess = 0.f;  
    
    ShapeComponentData m_ShapeData;

    std::vector<std::string> m_vObjectMeshList;
    std::vector<std::string> m_vObjectPrefabList;

    std::function<void(
        const char*, const char*, const char*, Vec3, Vec3, Vec3, Vec3, float, Vec3, float, ShapeComponentData)> m_OnCreate;

};
