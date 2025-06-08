#include "pch.h"
#include "ABeamActor.h"

Vec3 ABeamActor::Prepare(const Vec3& pos, const Vec3& baseVelocity)
{
    SetPosition(pos);
    SetGravity(0.0f);
    SetStartScale(Vec3(5.0f, 1.0f, 0.5f));
    SetEndScale(Vec3(13.0f, 1.0f, 3.5f));

    // ���� ���� ���� ȸ�� ����
    Vec3 dir = baseVelocity;
    dir.Normalize();
    float yaw = atan2f(dir.x, dir.z); // Z�� ���� Yaw

    SetRotation(Vec3(0, yaw, 0)); // Y�� ȸ���� ����

    return Vec3::Zero;
}

void ABeamActor::Tick()
{
    AEffectActor::Tick();

    if (!m_bRender)
        return;

    float t = 1.0f - (m_fRemainTime / m_fDuration);


}