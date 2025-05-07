#include "pch.h"
#include "ObjectEditorUI.h"

void ObjectEditorUI::DrawUI()
{
    
    ImGui::TextColored(ImVec4(1, 1, 0, 1), "Object Configuration");

    static const char* actorTypes[] = { "Character", "Enemy", "Object" };
    ImGui::Text("Actor Type");
    ImGui::Combo("##ActorType", &m_iCurrentActorType, actorTypes, IM_ARRAYSIZE(actorTypes));

    static const char* meshTypes[] = { "Skinned", "Static" };
    ImGui::Text("Mesh Type");
    ImGui::Combo("##MeshType", &m_iCurrentMeshType, meshTypes, IM_ARRAYSIZE(meshTypes));

    ImGui::Separator(); ImGui::Spacing();

    ImGui::TextColored(ImVec4(1, 1, 0, 1), "Transform");
    ImGui::InputFloat3("Position", m_fPosition);
    ImGui::InputFloat3("Rotation", m_fRotation);
    ImGui::InputFloat3("Scale", m_fScale);

    ImGui::Separator(); ImGui::Spacing();

    ImGui::TextColored(ImVec4(1, 1, 0, 1), "Asset Paths");
    ImGui::InputText("Texture", m_szTexturePath, IM_ARRAYSIZE(m_szTexturePath));
    ImGui::InputText("Shader", m_szShaderPath, IM_ARRAYSIZE(m_szShaderPath));
    ImGui::InputText("Obj", m_szObjPath, IM_ARRAYSIZE(m_szObjPath));

    ImGui::Spacing();

    if (ImGui::Button("Create", ImVec2(-1, 0)))
    {
        if (m_OnCreate)
        {
            m_OnCreate(
                m_iCurrentActorType,
                m_iCurrentMeshType,
                m_szTexturePath,
                m_szShaderPath,
                m_szObjPath,
                Vec3(m_fPosition[0], m_fPosition[1], m_fPosition[2]),
                Vec3(m_fRotation[0], m_fRotation[1], m_fRotation[2]),
                Vec3(m_fScale[0], m_fScale[1], m_fScale[2])
            );
        }
    }
}
