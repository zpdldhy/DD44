#include "pch.h"
#include "EffectEditorUI.h"
#include "imgui.h"
#include "PostProcessManager.h"

void EffectEditorUI::DrawUI()
{
    ImGui::TextColored(ImVec4(1, 1, 0, 1), "Effect Settings");

    // --- Target Selection ---
    static const char* cubeNames[] = { "Cube 1", "Cube 2" ,"Sword"};
    //static int selectedCube = 0;
    ImGui::Text("Target Cube");
    ImGui::Combo("##TargetCube", &m_iSelectedActor, cubeNames, IM_ARRAYSIZE(cubeNames));

    ImGui::Separator(); ImGui::Spacing();

    // --- Glow Settings ---
    ImGui::TextColored(ImVec4(1, 1, 0, 1), "Glow");
    static float glowPower = 0.0f;
    static float glowColor[3] = { 1.0f, 1.0f, 1.0f };
    ImGui::SliderFloat("Glow Power", &glowPower, 0.0f, 5.0f);
    ImGui::ColorEdit3("Glow Color", glowColor);


    // --- Emissive ---
    ImGui::TextColored(ImVec4(1, 1, 0, 1), "Emissive");
    static float emissiveColor[3] = { 1.0f, 1.0f, 1.0f };
    static float emissivePower = 0.0f;
    ImGui::ColorEdit3("Emissive Color", emissiveColor);
    ImGui::SliderFloat("Emissive Power", &emissivePower, 0.0f, 10.0f);

    ImGui::Spacing(); ImGui::Spacing();

    // --- Apply Effect ---
    if (ImGui::Button("Apply Effect", ImVec2(-1, 0)) && m_OnEffectApply)
    {
        m_OnEffectApply(
            m_iSelectedActor,
            glowPower,
            Vec3(glowColor[0], glowColor[1], glowColor[2]),
            Vec3(emissiveColor[0], emissiveColor[1], emissiveColor[2]),
            emissivePower
        );
    }

    static bool firstRun = true;
    if (firstRun && m_OnEffectApply)
    {
        m_OnEffectApply(
            m_iSelectedActor,
            glowPower,
            Vec3(glowColor[0], glowColor[1], glowColor[2]),
            Vec3(emissiveColor[0], emissiveColor[1], emissiveColor[2]),
            emissivePower
        );
        firstRun = false;
    }

    ImGui::Separator(); ImGui::Spacing();

    // --- Lighting Settings ---
    ImGui::TextColored(ImVec4(1, 1, 0, 1), "Lighting");

    static const char* lightTypes[] = { "Directional", "Point", "Spot" };
    static int selectedLightType = 0;
    static float lightColor[3] = { 1.0f, 1.0f, 1.0f };
    static float ambientColor[3] = { 0.1f, 0.1f, 0.1f };
    static float lightIntensity = 0.0f;
    static float ambientPower = 0.0f;

    ImGui::Text("Light Type");
    ImGui::Combo("##LightType", &selectedLightType, lightTypes, IM_ARRAYSIZE(lightTypes));

    ImGui::ColorEdit3("Light Color", lightColor);
    ImGui::SliderFloat("Light Intensity", &lightIntensity, 0.0f, 5.0f);

    ImGui::ColorEdit3("Ambient Color", ambientColor);
    ImGui::SliderFloat("Ambient Power", &ambientPower, 0.0f, 2.0f);

    ImGui::Spacing();

    if (ImGui::Button("Apply Light", ImVec2(-1, 0)) && m_OnLightApply)
    {
        m_OnLightApply(
            static_cast<ELightType>(selectedLightType),
            Vec3(lightColor[0], lightColor[1], lightColor[2]),
            lightIntensity,
            Vec3(ambientColor[0], ambientColor[1], ambientColor[2]),
            ambientPower
        );
    }

    // --- Debug View Mode ---
    ImGui::Separator(); ImGui::Spacing();
    ImGui::TextColored(ImVec4(1, 1, 0, 1), "Debug View Mode");

    static const char* debugModes[] =
    {
        "Final Output", "Original", "Blur", "Blur Mask", "Bloom",
        "Bloom Mask", "Light", "Normal", "Edge Detection"
    };

    static int selectedDebugMode = 0;
    ImGui::Combo("Debug Mode", &selectedDebugMode, debugModes, IM_ARRAYSIZE(debugModes));

    // ¿¬µ¿
    POSTPROCESS->SetDebugMode(selectedDebugMode);
}