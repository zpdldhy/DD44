#include "pch.h"
#include "ABeamActor.h"
#include "Timer.h"

Vec3 ABeamActor::Prepare(const Vec3& _pos, const Vec3& baseVelocity)
{
    Vec3 pos = _pos;
    //pos.y += 5.f;
    SetPosition(pos);
    SetGravity(0.0f);
    SetStartScale(Vec3(5.0f, 1.0f, 0.5f));
    SetEndScale(Vec3(13.0f, 1.0f, 3.5f));

    // 방향 벡터 기준 회전 적용
    Vec3 dir = baseVelocity;
    dir.Normalize();
    float yaw = atan2f(dir.x, dir.z);
    float pitch = -asinf(dir.y); // 위로 향할수록 음수 (카메라 pitch처럼)

    SetRotation(Vec3(pitch, yaw, 0));
    m_vVelocity = Vec3(0, 0, 0);

    return m_vVelocity; 
}

void ABeamActor::Tick()
{
    AEffectActor::Tick();

    if (!m_bRender)
        return;

    float t = 1.0f - (m_fRemainTime / m_fDuration);

}