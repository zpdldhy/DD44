#include "pch.h"
#include "AFeatherActor.h"
#include "Timer.h"

Vec3 AFeatherActor::Prepare(const Vec3& pos, const Vec3& baseVelocity, float _scale)
{
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

    SetGravity(1.0f);

    SetStartScale(Vec3(0.5f));
    SetEndScale(Vec3(1.2f));
    InitSpriteAnimation(1, 0.5f);

    SetPosition(pos);

    m_bReversePlay = true;

    return finalVelocity;
}

void AFeatherActor::Tick()
{
    AEffectActor::Tick(); 

    Vec3 pos = GetPosition();

    float time = TIMER->GetGameTime(); 
    pos.x += sin(time * 3.0f) * 0.2f; 

    SetPosition(pos);

    Vec3 rot = GetRotation();
    rot.z += 1.0f * TIMER->GetDeltaTime();
    SetRotation(rot);
}
