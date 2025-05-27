#pragma once
#include "AParticleActor.h"
#include "EffectType.h"

class AEffectActor : public AParticleActor
{
public:
    AEffectActor(EEffectType type) : m_eType(type) {}

    EEffectType GetType() const { return m_eType; }

    void Play(float duration, const Vec3& velocity);
    virtual void Tick() override;
    bool IsActive() const { return m_bRender && !m_bDelete; }

public:
    void SetGravity(float gravity) { m_fGravity = gravity; }
    void SetStartScale(const Vec3& scale) { m_vStartScale = scale; }
    void SetEndScale(const Vec3& scale) { m_vEndScale = scale; }

private:
    EEffectType m_eType;
    float m_fRemainTime = 0.0f;
    Vec3 m_vVelocity = Vec3(0, 0, 0);

    float m_fStartScale = 0.2f;
    float m_fEndScale = 1.0f;
    float m_fGravity = -30.0f;

    Vec3 m_vStartScale = Vec3(.1f, .1f, 5.1f);
    Vec3 m_vEndScale = Vec3(3.f, .2f, 5.1f);
};