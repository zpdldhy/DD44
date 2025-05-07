#pragma once
#include <functional>

class EffectEditorUI
{
public:
    void DrawUI();

    void SetEffectApplyCallback(std::function<void(int, float, Vec3, float)> callback)
    {
        m_OnEffectApply = std::move(callback);
    }

    float GetGlowPower() const { return m_fGlowPower; }
    float GetDissolveThreshold() const { return m_fDissolveThreshold; }
    Vec3 GetGlowColor() const { return Vec3(m_vGlowColor[0], m_vGlowColor[1], m_vGlowColor[2]); }

    int GetSelectedActor() const { return m_iSelectedActor; }

private:
    int m_iSelectedActor = 0;
    float m_fGlowPower = 0;
    float m_fDissolveThreshold = 0;
    float m_vGlowColor[3] = { 1.0f, 1.0f, 1.0f };

    std::function<void(int, float, Vec3, float)> m_OnEffectApply;
};