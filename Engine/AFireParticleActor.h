#pragma once
#include "AParticleActor.h"
class AFireParticleActor : public AParticleActor
{
public:
    AFireParticleActor();
    virtual ~AFireParticleActor() = default;

    virtual void Tick() override;

    Vec3 GetAmplitude() { return m_vAmplitude; }
    Vec3 GetRandomFreq() { return m_vRandomFreq; }
    Vec3 GetRandomOffset() { return m_vRandomOffset; }
    float GetTimeOffset() { return m_fTimeOffset; }

    void SetAmplitude(const Vec3& _Amplitude) { m_vAmplitude = _Amplitude; }
    void SetRandomFreq(const Vec3& _RandomFreq) { m_vRandomFreq = _RandomFreq; }
    void SetRandomOffset(const Vec3& _RandomOffset) { m_vRandomOffset = _RandomOffset; }
    void SetTimeOffset(float _TimeOffset) { m_fTimeOffset = _TimeOffset; }

private: 
    Vec3 m_vAmplitude;
    Vec3 m_vRandomFreq;
    Vec3 m_vRandomOffset;

    float m_fTimeOffset = 0.0f;
    float m_fLocalTime = 0.0f;
    float m_fFlickerValue = 0.0f;

    Vec3 m_vBasePosition;
    bool m_bBaseInitialized;
    
};