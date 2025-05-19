#include "pch.h"
#include "UIEditorUI.h"
#include "PrefabLoader.h"

void UIEditorUI::DrawUI()
{
    // ───────────────────────────── Transform ─────────────────────────────
    ImGui::TextColored(ImVec4(1, 1, 0, 1), "Transform");
    DrawVec3Slider("Position", m_fPosition, -100.f, 100.f);
    DrawVec3Slider("Rotation", m_fRotation, -360.f, 360.f);
    DrawVec3Slider("Scale", m_fScale, 0.01f, 10.f);

    ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing();
    // ───────────────────────────── Asset Input ─────────────────────────────
    ImGui::TextColored(ImVec4(1, 1, 0, 1), "Asset Paths");
    ImGui::InputText("Texture", m_szTexturePath, IM_ARRAYSIZE(m_szTexturePath));
    ImGui::InputText("Shader", m_szShaderPath, IM_ARRAYSIZE(m_szShaderPath));

    ImGui::Spacing();

    if (ImGui::Button("Create", ImVec2(-1, 0)) && m_OnCreate)
    {
        m_OnCreate(
            m_szTexturePath,
            m_szShaderPath,
            Vec3(m_fPosition[0], m_fPosition[1], m_fPosition[2]),
            Vec3(m_fRotation[0], m_fRotation[1], m_fRotation[2]),
            Vec3(m_fScale[0], m_fScale[1], m_fScale[2])
        );
    }

    // ───────────────────────────── Prefab Save/Load ─────────────────────────────
    ImGui::TextColored(ImVec4(1, 1, 0, 1), "Prefab Tools");

    static char prefabName[64] = "MyUI";
    ImGui::InputText("Prefab Name", prefabName, IM_ARRAYSIZE(prefabName));

    if (ImGui::Button("Save as Prefab", ImVec2(-1, 0)))
    {
        PrefabObjectData data;
        data.Name = prefabName;
        data.ShaderPath = m_szShaderPath;
        data.TexturePath = m_szTexturePath;
        data.Scale = Vec3(m_fScale[0], m_fScale[1], m_fScale[2]);
        data.Rotation = Vec3(m_fRotation[0], m_fRotation[1], m_fRotation[2]);
        data.Translation = Vec3(m_fPosition[0], m_fPosition[1], m_fPosition[2]);

        PrefabLoader().SaveObject(data, "../Resources/Prefab/" + data.Name + ".ui.json");
    }

    if (ImGui::Button("Load Prefab##File", ImVec2(-1, 0)))
    {
        PrefabObjectData data;
        std::string path = "../Resources/Prefab/" + std::string(prefabName) + ".ui.json";
        if (PREFAB->LoadObject(path, data))
        {
            strcpy_s(m_szShaderPath, data.ShaderPath.c_str());
            strcpy_s(m_szTexturePath, data.TexturePath.c_str());

            m_fScale[0] = data.Scale.x;    m_fScale[1] = data.Scale.y;    m_fScale[2] = data.Scale.z;
            m_fRotation[0] = data.Rotation.x; m_fRotation[1] = data.Rotation.y; m_fRotation[2] = data.Rotation.z;
            m_fPosition[0] = data.Translation.x; m_fPosition[1] = data.Translation.y; m_fPosition[2] = data.Translation.z;
        }
    }

    ImGui::Separator(); ImGui::Spacing();
}

void UIEditorUI::DrawVec3Slider(const char* label, float* values, float minVal, float maxVal)
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
