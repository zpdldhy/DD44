#pragma once
#include <functional>
#include "ULightComponent.h"

class EffectEditorUI
{
public:
    void DrawUI();

    void SetEffectApplyCallback(std::function<void(int, float, Vec3, Vec3, float)> callback)
    {
        m_OnEffectApply = std::move(callback);
    }
    
    void SetLightApplyCallback(std::function<void(ELightType, Vec3, float, Vec3, float)> callback)
    {
        m_OnLightApply = std::move(callback);
    }

    // 이펙트 관련 Getter
    float GetGlowPower() const { return m_fGlowPower; }
    float GetDissolveThreshold() const { return m_fDissolveThreshold; }
    Vec3 GetGlowColor() const { return Vec3(m_vGlowColor[0], m_vGlowColor[1], m_vGlowColor[2]); }
    int GetSelectedActor() const { return m_iSelectedActor; }
    Vec3 GetEmissiveColor() const { return Vec3(m_vEmissiveColor[0], m_vEmissiveColor[1], m_vEmissiveColor[2]);}

    // 라이트 관련 Getter
    ELightType GetSelectedLightType() const { return static_cast<ELightType>(m_iSelectedLightType); }
    Vec3 GetLightColor() const { return Vec3(m_vLightColor[0], m_vLightColor[1], m_vLightColor[2]); }
    float GetLightIntensity() const { return m_fLightIntensity; }
    Vec3 GetAmbientColor() const { return Vec3(m_vAmbientColor[0], m_vAmbientColor[1], m_vAmbientColor[2]); }
    float GetAmbientPower() const { return m_fAmbientPower; }

private:
    // 이펙트 파라미터
    int m_iSelectedActor = 0;
    float m_fGlowPower = 0;
    float m_fDissolveThreshold = 0;
    float m_vGlowColor[3] = { 1.0f, 1.0f, 1.0f };
    float m_vEmissiveColor[3] = { 1.0f, 1.0f, 1.0f };
    float m_fEmissivePower = 0.0f;

    std::function<void(int, float, Vec3, Vec3, float)> m_OnEffectApply;

    // 광원 파라미터
    int m_iSelectedLightType = 0;
    float m_vLightColor[3] = { 1.0f, 1.0f, 1.0f };
    float m_fLightIntensity = 1.0f;
    float m_vAmbientColor[3] = { 0.2f, 0.2f, 0.2f };
    float m_fAmbientPower = 1.0f;

    std::function<void(ELightType, Vec3, float, Vec3, float)> m_OnLightApply;
};