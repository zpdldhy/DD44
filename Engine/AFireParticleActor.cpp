#include "pch.h"
#include "AFireParticleActor.h"
#include "Timer.h"

AFireParticleActor::AFireParticleActor()
{
    m_vAmplitude = Vec3(RandomRange(0.1f, 0.5f), RandomRange(0.2f, 0.8f), 0.0f);
    m_vRandomFreq = Vec3(RandomRange(1.0f, 3.0f), RandomRange(1.5f, 4.0f), 0.0f);
    m_vRandomOffset = Vec3(RandomRange(0.0f, 100.0f), RandomRange(0.0f, 100.0f), 0.0f);

    m_bBaseInitialized = false;
    m_fTimeOffset = RandomRange(0.0f, 100.0f); 
    m_fLocalTime = 0.0f;
}

void AFireParticleActor::Tick()
{
    this->GetMeshComponent()->SetInstanceColor(Vec4(1, 1, 1, 1));
    AParticleActor::Tick(); // 기본 애니메이션 등 유지
    if (!m_bBaseInitialized)
    {
        m_vBasePosition = GetPosition();
        m_bBaseInitialized = true;
    }

    m_fLocalTime += TIMER->GetDeltaTime();

    float t = m_fLocalTime + m_fTimeOffset;

    float x = sinf(t * m_vRandomFreq.x) * m_vAmplitude.x;
    float y = sinf(t * m_vRandomFreq.y) * m_vAmplitude.y;

    float targetFlicker = RandomRange(-0.03f, 0.03f);
    m_fFlickerValue = Lerp(m_fFlickerValue, targetFlicker, TIMER->GetDeltaTime() * 5.0f);

    SetPosition(m_vBasePosition + Vec3(x, y + m_fFlickerValue, 0));

   /* float rotZ = sinf(t) * 0.2f; 
    SetRotation(Vec3(0, 0, rotZ)); */
}