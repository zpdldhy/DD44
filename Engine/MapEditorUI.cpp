#include "pch.h"
#include "MapEditorUI.h"
#include "PrefabLoader.h"

void MapEditorUI::DrawUI()
{
    ImGui::TextColored(ImVec4(1, 1, 0, 1), "Terrain Configuration");
    ImGui::InputInt("Num Cols", &m_iCols);
    ImGui::InputInt("Num Rows", &m_iRows);
    ImGui::InputFloat("Cell Size", &m_fCellSize);

    ImGui::Separator(); ImGui::Spacing();

    ImGui::TextColored(ImVec4(1, 1, 0, 1), "Transform");
    ImGui::InputFloat3("Position", m_fPosition);
    ImGui::InputFloat3("Rotation", m_fRotation);
    ImGui::InputFloat3("Scale", m_fScale);

    ImGui::Separator(); ImGui::Spacing();

    ImGui::TextColored(ImVec4(1, 1, 0, 1), "Appearance");
    ImGui::InputText("Texture File", m_szTextureName, IM_ARRAYSIZE(m_szTextureName));
    ImGui::InputText("Shader File", m_szShaderName, IM_ARRAYSIZE(m_szShaderName));

    ImGui::Separator(); ImGui::Spacing();

    ImGui::InputInt("Row", &m_SelectedRow);
    ImGui::InputInt("Col", &m_SelectedCol);
    ImGui::SliderFloat("Height", &m_TargetHeight, -100.0f, 100.0f);

    ImGui::Separator(); ImGui::Spacing();

    if (ImGui::Button("Create Terrain Tile", ImVec2(-1, 0)))
    {
        if (m_OnCreate)
        {
            m_OnCreate();
        }
    }

    ImGui::Separator(); ImGui::Spacing();

    ImGui::TextColored(ImVec4(1, 1, 0, 1), "Tile Save/Load");

    static char SaveFileName[256] = "MyTile.map.json";
    ImGui::InputText("Save File Name", SaveFileName, IM_ARRAYSIZE(SaveFileName));

    // 저장
    if (ImGui::Button("Save##Tile"))
    {
        std::string textureFullPath = "../Resources/Texture/" + std::string(m_szTextureName);
        std::string shaderFullPath = "../Resources/Shader/" + std::string(m_szShaderName);

        PrefabMapData data;
        data.Cols = m_iCols;
        data.Rows = m_iRows;
        data.CellSize = m_fCellSize;
        data.Position = GetPosition();
        data.Rotation = GetRotation();
        data.Scale = GetScale();
        data.SelectedRow = m_SelectedRow;
        data.SelectedCol = m_SelectedCol;
        data.TargetHeight = m_TargetHeight;
        data.TexturePath = textureFullPath;
        data.ShaderPath = shaderFullPath;

        PREFAB->SaveMapTile(data, std::string("../Resources/Prefab/") + SaveFileName);
    }

    static char LoadFileName[256] = "MyTile.map.json";
    ImGui::InputText("Load File Name", LoadFileName, IM_ARRAYSIZE(LoadFileName));

    // 불러오기
    if (ImGui::Button("Load##Tile"))
    {
        std::string path = std::string("../Resources/Prefab/") + LoadFileName;

        PrefabMapData data;
        if (PREFAB->LoadMapTile(path, data))
        {
            m_iCols = data.Cols;
            m_iRows = data.Rows;
            m_fCellSize = data.CellSize;
            m_fPosition[0] = data.Position.x;
            m_fPosition[1] = data.Position.y;
            m_fPosition[2] = data.Position.z;
            m_fRotation[0] = data.Rotation.x;
            m_fRotation[1] = data.Rotation.y;
            m_fRotation[2] = data.Rotation.z;
            m_fScale[0] = data.Scale.x;
            m_fScale[1] = data.Scale.y;
            m_fScale[2] = data.Scale.z;
            m_SelectedRow = data.SelectedRow;
            m_SelectedCol = data.SelectedCol;
            m_TargetHeight = data.TargetHeight;
            strcpy_s(m_szTextureName, data.TexturePath.c_str());
            strcpy_s(m_szShaderName, data.ShaderPath.c_str());
        }
    }

    ImGui::Separator(); ImGui::Spacing();
    ImGui::TextColored(ImVec4(1, 1, 0, 1), "Map File Browser");

    m_vMapPrafabList = PREFAB->GetPrefabFileNames("../Resources/Prefab/", ".map.json");
    static std::string selectedMapFileName = "";

    for (const auto& name : m_vMapPrafabList)
    {
        if (ImGui::Selectable(name.c_str(), selectedMapFileName == name))
            selectedMapFileName = name;
    }

    if (!selectedMapFileName.empty() && ImGui::Button("Load Selected Map"))
    {
        PrefabMapData data;
        std::string path = "../Resources/Prefab/" + selectedMapFileName + ".json";
        if (PREFAB->LoadMapTile(path, data))
        {
            m_iCols = data.Cols;
            m_iRows = data.Rows;
            m_fCellSize = data.CellSize;
            m_fPosition[0] = data.Position.x;
            m_fPosition[1] = data.Position.y;
            m_fPosition[2] = data.Position.z;
            m_fRotation[0] = data.Rotation.x;
            m_fRotation[1] = data.Rotation.y;
            m_fRotation[2] = data.Rotation.z;
            m_fScale[0] = data.Scale.x;
            m_fScale[1] = data.Scale.y;
            m_fScale[2] = data.Scale.z;
            m_SelectedRow = data.SelectedRow;
            m_SelectedCol = data.SelectedCol;
            m_TargetHeight = data.TargetHeight;
            strcpy_s(m_szTextureName, data.TexturePath.c_str());
            strcpy_s(m_szShaderName, data.ShaderPath.c_str());
        }
    }

    ImGui::Separator(); ImGui::Spacing();

    if (ImGui::CollapsingHeader("Splatting Brush"))
    {
        ImGui::Checkbox("Splatting Mode", &m_bSplattingMode);
        ImGui::SliderFloat("Brush Radius", &m_fBrushRadius, 1.0f, 50.0f);
        ImGui::SliderFloat("Brush Strength", &m_fBrushStrength, 0.0f, 1.0f);

        const char* channels[] = { "Red", "Green", "Blue", "Alpha" };
        ImGui::Combo("Target Channel", &m_iBrushChannel, channels, IM_ARRAYSIZE(channels));
    }
}

std::wstring MapEditorUI::GetTexturePath() const
{
    return L"../Resources/Texture/" + std::wstring(m_szTextureName, m_szTextureName + strlen(m_szTextureName));
}

std::wstring MapEditorUI::GetShaderPath() const
{
    return L"../Resources/Shader/" + std::wstring(m_szShaderName, m_szShaderName + strlen(m_szShaderName));
}
