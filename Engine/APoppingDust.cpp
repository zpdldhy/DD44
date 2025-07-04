#include "pch.h"
#include "APoppingDust.h"

Vec3 APoppingDust::Prepare(const Vec3& pos, const Vec3& baseVelocity, float scale)
{
    // 방향은 baseVelocity 기반으로 약간 랜덤 회전
    Vec3 dir = baseVelocity;
    dir.Normalize();

    float spreadYaw = DegreesToRadians(RandomRange(-15.0f, 15.0f));
    float spreadPitch = DegreesToRadians(RandomRange(-10.0f, 10.0f));

    Matrix rotYaw = Matrix::CreateRotationY(spreadYaw);
    Matrix rotPitch = Matrix::CreateRotationX(spreadPitch);

    Vec3 finalDir = Vec3::TransformNormal(dir, rotYaw * rotPitch);
    finalDir.Normalize();

    float speedFactor = RandomRange(0.3f, 0.6f);
    Vec3 finalVelocity = finalDir * baseVelocity.Length() * speedFactor;

    // 약한 gravity
    SetGravity(3.0f);

    // 작게 시작해서 부드럽게 사라짐
    SetStartScale(Vec3(5.5f) * scale);
    SetEndScale(Vec3(8.5f) * scale); // 아주 부드럽게 퍼지게

    InitSpriteAnimation(4, 0.5f);

    SetPosition(pos);

    m_bReversePlay = true;

    return finalVelocity;
}
