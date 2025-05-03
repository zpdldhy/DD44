#include "pch.h"
#include "MapEditorUI.h"
#include "imgui.h"

void MapEditorUI::Update()
{
    ImGui::Begin("Map Editor");

    ImGui::TextColored(ImVec4(1, 1, 1, 1), "Terrain Configuration");
    ImGui::InputInt("Num Cols", &m_iCols);
    ImGui::InputInt("Num Rows", &m_iRows);
    ImGui::InputFloat("Cell Size", &m_fCellSize);

    ImGui::Separator(); ImGui::Spacing();

    ImGui::TextColored(ImVec4(1, 1, 1, 1), "Transform");
    ImGui::InputFloat3("Position", m_fPosition);
    ImGui::InputFloat3("Rotation", m_fRotation);
    ImGui::InputFloat3("Scale", m_fScale);

    ImGui::Separator(); ImGui::Spacing();

    ImGui::TextColored(ImVec4(1, 1, 1, 1), "Appearance");
    ImGui::InputText("Texture File", m_szTextureName, IM_ARRAYSIZE(m_szTextureName));
    ImGui::InputText("Shader File", m_szShaderName, IM_ARRAYSIZE(m_szShaderName));

    ImGui::Spacing();

    if (ImGui::Button("Create Terrain Tile", ImVec2(-1, 0)))
    {
        if (m_OnCreate) m_OnCreate();
    }

    ImGui::End();
}

std::wstring MapEditorUI::GetTexturePath() const
{
    return L"../Resources/Texture/" + std::wstring(m_szTextureName, m_szTextureName + strlen(m_szTextureName));
}

std::wstring MapEditorUI::GetShaderPath() const
{
    return L"../Resources/Shader/" + std::wstring(m_szShaderName, m_szShaderName + strlen(m_szShaderName));
}
