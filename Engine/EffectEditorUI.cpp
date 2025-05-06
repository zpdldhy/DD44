#include "pch.h"
#include "EffectEditorUI.h"
#include "imgui.h"

void EffectEditorUI::Update()
{
    // [1] ÀÌÆåÆ® °ü·Ã UI
    ImGui::Begin("Effect Editor");

    static const char* cubeNames[] = { "Cube 1", "Cube 2" };
    ImGui::Combo("Target Cube", &m_iSelectedActor, cubeNames, IM_ARRAYSIZE(cubeNames));

    ImGui::SliderFloat("Glow Power", &m_fGlowPower, 0.0f, 5.0f);
    ImGui::ColorEdit3("Glow Color", m_vGlowColor);
    ImGui::SliderFloat("Dissolve", &m_fDissolveThreshold, 0.0f, 1.0f);

    ImGui::ColorEdit3("Emissive Color", m_vEmissiveColor);
    ImGui::SliderFloat("Emissive Power", &m_fEmissivePower, 0.0f, 10.0f);

    if (ImGui::Button("Apply Effect") && m_OnEffectApply)
    {
        m_OnEffectApply(m_iSelectedActor, 
            m_fGlowPower,
            GetGlowColor(),
            m_fDissolveThreshold,
            GetEmissiveColor(),
            m_fEmissivePower);
    }

    static bool bFirstRun = true;
    if (bFirstRun && m_OnEffectApply)
    {
        m_OnEffectApply(m_iSelectedActor,
            m_fGlowPower,
            GetGlowColor(),
            m_fDissolveThreshold,
            GetEmissiveColor(),
            m_fEmissivePower);
        bFirstRun = false;
    }

    ImGui::End();

    // [2] ±¤¿ø ¼³Á¤ UI
    ImGui::Begin("Light Editor");

    static const char* lightTypes[] = { "Directional", "Point", "Spot" };
    ImGui::Combo("Light Type", &m_iSelectedLightType, lightTypes, IM_ARRAYSIZE(lightTypes));

    ImGui::ColorEdit3("Light Color", m_vLightColor);
    ImGui::SliderFloat("Light Intensity", &m_fLightIntensity, 0.0f, 5.0f);

    ImGui::ColorEdit3("Ambient Color", m_vAmbientColor);
    ImGui::SliderFloat("Ambient Power", &m_fAmbientPower, 0.0f, 2.0f);

    if (ImGui::Button("Apply Light") && m_OnLightApply)
    {
        m_OnLightApply(
            static_cast<ELightType>(m_iSelectedLightType),
            GetLightColor(),
            m_fLightIntensity,
            GetAmbientColor(),
            m_fAmbientPower
        );
    }

    ImGui::End();
}