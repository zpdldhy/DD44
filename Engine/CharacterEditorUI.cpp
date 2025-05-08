#include "pch.h"
#include "CharacterEditorUI.h"
#include "ActorLoader.h"

void CharacterEditorUI::DrawUI()
{
    ImGui::TextColored(ImVec4(1, 1, 0, 1), "Asset");
    ImGui::InputText("Asset Path", m_szAssetPath, IM_ARRAYSIZE(m_szAssetPath));

    ImGui::TextColored(ImVec4(1, 1, 0, 1), "Character Configuration");

    // Actor Type
    static const char* actorTypes[] = { "Character", "Enemy" };
    ImGui::Text("Actor Type"); ImGui::SameLine(100);
    ImGui::Combo("##ActorType", &m_iActorType, actorTypes, IM_ARRAYSIZE(actorTypes));

    // Component Type
    static const char* componentTypes[] = { "Skinned", "Static" };
    ImGui::Text("Component"); ImGui::SameLine(100);
    ImGui::Combo("##ComponentType", &m_iComponentType, componentTypes, IM_ARRAYSIZE(componentTypes));

    ImGui::Separator(); ImGui::Spacing();

    // Transform
    ImGui::TextColored(ImVec4(1, 1, 0, 1), "Transform");
    DrawVec3Slider("Position", m_fPosition, -100.f, 100.f);
    DrawVec3Slider("Rotation", m_fRotation, -360.f, 360.f);
    DrawVec3Slider("Scale", m_fScale, 0.01f, 10.f);

    ImGui::Spacing();

    // Script
    static const char* scriptTypes[] = { "None", "PlayerMoveScript", "EnemyAIScript" };
    ImGui::Text("Script"); ImGui::SameLine(100);
    ImGui::Combo("##ScriptType", &m_iScriptType, scriptTypes, IM_ARRAYSIZE(scriptTypes));

    ImGui::Spacing();

    if (ImGui::Button("Create", ImVec2(-1, 0)) && m_OnCreate)
    {
        m_OnCreate(
            m_iActorType,
            m_iComponentType,
            m_szAssetPath,
            Vec3(m_fPosition[0], m_fPosition[1], m_fPosition[2]),
            Vec3(m_fRotation[0], m_fRotation[1], m_fRotation[2]),
            Vec3(m_fScale[0], m_fScale[1], m_fScale[2]),
            m_iScriptType
        );
    }
}
void CharacterEditorUI::DrawVec3Slider(const char* label, float* values, float minVal, float maxVal)
{
    ImGui::PushID(label);
    ImGui::Text("%s", label);
    ImGui::Indent(10.0f);

    static const char* axes[] = { "X", "Y", "Z" };
    const float buttonSize = 20.0f;
    const float inputWidth = 60.0f;
    const float sliderWidth = 150.0f;
    const float spacing = 4.0f;

    for (int i = 0; i < 3; ++i)
    {
        ImGui::PushID(i);

        ImGui::Text("%s", axes[i]); ImGui::SameLine();

        if (ImGui::Button("-", ImVec2(buttonSize, 0)))
            values[i] -= 0.1f;
        ImGui::SameLine(0, spacing);

        ImGui::PushItemWidth(sliderWidth);
        if (ImGui::SliderFloat("##Slider", &values[i], minVal, maxVal))
        {
            // 값이 바뀐 경우 추가 작업 가능
        }
        ImGui::PopItemWidth();

        ImGui::SameLine(0, spacing);

        if (ImGui::Button("+", ImVec2(buttonSize, 0)))
            values[i] += 0.1f;

        ImGui::SameLine(0, spacing);

        ImGui::PushItemWidth(inputWidth);
        ImGui::InputFloat("##Value", &values[i], 0.0f, 0.0f, "%.2f");
        ImGui::PopItemWidth();

        ImGui::PopID();
    }

    ImGui::Unindent(10.0f);
    ImGui::PopID();
}

