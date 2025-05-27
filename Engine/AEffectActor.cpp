#include "pch.h"
#include "AEffectActor.h"
#include "Timer.h"

void AEffectActor::Play(float duration, const Vec3& velocity)
{
	InitSpriteAnimation(m_iDivisions, duration);
    m_fRemainTime = duration;
    m_vVelocity = velocity;
    SetRender(true);

    SetScale(m_vStartScale);
}

void AEffectActor::Tick()
{
    AActor::Tick();

    if (!m_bRender)
        return;

    m_vVelocity.y += m_fGravity * TIMER->GetDeltaTime();


    Vec3 pos = GetPosition();
    pos += m_vVelocity * TIMER->GetDeltaTime();
    SetPosition(pos);

    float t = 1.0f - (m_fRemainTime / m_fDuration);// 진행 비율 (0~1)
    Vec3 scale;
    scale.x = Lerp(m_vStartScale.x, m_vEndScale.x, t);
    scale.y = Lerp(m_vStartScale.y, m_vEndScale.y, t);
    scale.z = 1.0f;

    m_fRemainTime -= TIMER->GetDeltaTime();
    if (m_fRemainTime <= 0.0f)
    {
        SetRender(false);
    }
}
