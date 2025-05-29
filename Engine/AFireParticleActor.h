#pragma once
#include "AParticleActor.h"
class AFireParticleActor : public AParticleActor
{
public:
    AFireParticleActor();
    virtual ~AFireParticleActor() = default;

    virtual void Tick() override;

private:
    //Vec3 m_vBasePosition;
    //Vec3 m_vAmplitude = { 1.0f, 1.5f, 0.0f }; // 흔들림 강도
    //float m_fSpeed = 2.0f;                   // 진동 속도
    //float m_fOffset = 0.0f;                  // 위상차
    //bool m_bBaseInitialized = false;

    Vec3 m_vBasePosition;
    Vec3 m_vAmplitude;
    Vec3 m_vRandomFreq;
    Vec3 m_vRandomOffset;

    float m_fFlickerValue = 0.0f;
    float m_fTimeOffset = 0.0f;

    bool m_bBaseInitialized;
};