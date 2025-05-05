#include "pch.h"
#include "EffectEditorUI.h"
#include "imgui.h"

void EffectEditorUI::Update()
{
    ImGui::Begin("Effect Editor");

    ////¹öÆ°
    //if (ImGui::Button("Cube 1"))    m_iSelectedActor = 0;
    //ImGui::SameLine();
    //if (ImGui::Button("Cube 2"))    m_iSelectedActor = 1;

    static const char* cubeNames[] = { "Cube 1", "Cube 2" };
    ImGui::Combo("Target Cube", &m_iSelectedActor, cubeNames, IM_ARRAYSIZE(cubeNames));


    ImGui::SliderFloat("Glow Power", &m_fGlowPower, 0.0f, 5.0f);
    ImGui::ColorEdit3("Glow Color", m_vGlowColor);
    ImGui::SliderFloat("Dissolve", &m_fDissolveThreshold, 0.0f, 1.0f);

    if (ImGui::Button("Apply Effect") && m_OnEffectApply)
    {
        m_OnEffectApply(m_iSelectedActor, m_fGlowPower, GetGlowColor(), m_fDissolveThreshold);
    }

    static bool bFirstRun = true;
    if (bFirstRun && m_OnEffectApply)
    {
        m_OnEffectApply(m_iSelectedActor, m_fGlowPower, GetGlowColor(), m_fDissolveThreshold);
        bFirstRun = false;
    }

    ImGui::End();
  
}