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
    m_vVelocity *= 0.95f;
    m_vVelocity.y += m_fGravity * TIMER->GetDeltaTime();


    Vec3 pos = GetPosition();
    pos += m_vVelocity * TIMER->GetDeltaTime();
    SetPosition(pos);

    /* m_vVelocity.x += RandomRange(-1.0f, 1.0f);
     m_vVelocity.y += RandomRange(-1.0f, 1.0f);*/

    float t = 1.0f - (m_fRemainTime / m_fDuration);// 진행 비율 (0~1)
    t = pow(t, 2);
    Vec3 scale;
    scale.x = Lerp(m_vStartScale.x, m_vEndScale.x, t);
    scale.y = Lerp(m_vStartScale.y, m_vEndScale.y, pow(t, 2.5));
    scale.z = 1.0f;
    SetScale(scale);

    m_fRemainTime -= TIMER->GetDeltaTime();
    if (m_fRemainTime <= 0.0f)
    {
        SetRender(false);
    }
}
