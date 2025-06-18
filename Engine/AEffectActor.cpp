#include "pch.h"
#include "AEffectActor.h"
#include "Timer.h"


void AEffectActor::Play(float duration, const Vec3& velocity)
{
	InitSpriteAnimation(m_iDivisions, duration);
    m_fRemainTime = duration;
    m_vVelocity = velocity;
    m_bRender = true;

    SetScale(m_vStartScale);
}

void AEffectActor::Tick()
{
    AParticleActor::Tick();

    if (!m_bRender)
        return;

    m_vVelocity *= 0.97f;
    m_vVelocity.y += m_fGravity * TIMER->GetDeltaTime();

    Vec3 pos = GetPosition();
    pos += m_vVelocity * TIMER->GetDeltaTime();
    SetPosition(pos);

    // billboard용 회전 행렬은 그대로 두고 위치만 덮어쓰기
    Matrix mat = GetWorld();
    mat._41 = pos.x;
    mat._42 = pos.y;
    mat._43 = pos.z;
    SetWorldMatrix(mat); // g_matWorld에 반영될 matrix

    float t = 1.0f - (m_fRemainTime / m_fDuration);// 진행 비율 (0~1)
    //t = pow(t, 1.5);
    float growT = min(t * 2.0f, 1.0f); // 0~0.5초까지만 0~1로 증가 후 고정
    float endFactor = pow(1.0f - t, 2.5f);

    Vec3 scale;
    scale.x = Lerp(m_vStartScale.x, m_vEndScale.x * 1.f, growT);
    scale.y = Lerp(m_vStartScale.y, m_vEndScale.y * 1.f, growT);
    scale.z = Lerp(m_vStartScale.z, m_vEndScale.z * 1.f, growT);
    SetScale(scale);

    m_fRemainTime -= TIMER->GetDeltaTime();
    if (m_fRemainTime <= 0.0f)
    {
        m_bRender = false;
    }
}

Vec3 AEffectActor::Prepare(const Vec3& _pos, const Vec3& _baseVelocity, float _scale)
{
    return _baseVelocity;
}


