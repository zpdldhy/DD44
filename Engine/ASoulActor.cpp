#include "pch.h"
#include "ASoulActor.h"
#include "Timer.h"

Vec3 ASoulActor::Prepare(const Vec3& _pos, const Vec3& _baseVelocity, float _scale)
{
    Vec3 pos = _pos;
    SetPosition(pos);
    Vec3 velocity = _baseVelocity;
    velocity.y += 1;
    velocity *= 5;
    SetGravity(-3.0f);
    SetStartScale(Vec3(.5f) * _scale);
    SetEndScale(Vec3(.5f) * _scale);
    InitSpriteAnimation(1, 0.6f);
    return velocity;
}

void ASoulActor::SetTarget(const Vec3& _targetPos)
{
    m_vTargetPos = _targetPos;
}


void ASoulActor::Tick()
{
    AEffectActor::Tick();
    float a = (m_vTargetPos - GetPosition()).Length();
    

    wchar_t buffer[100];
    swprintf_s(buffer, L"Soul distance to target: %.3f\n", a);
    OutputDebugString(buffer);


    if ((m_vTargetPos - GetPosition()).Length()< 2.0f)
    {
        m_bRender = false;
    }
}
