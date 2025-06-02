#include "pch.h"
#include "AFeatherActor.h"
#include "Timer.h"

Vec3 AFeatherActor::Prepare(const Vec3& pos, const Vec3& baseVelocity)
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
    SetGravity(1.0f);

    // 작게 시작해서 부드럽게 사라짐
    SetStartScale(Vec3(0.5f));
    SetEndScale(Vec3(1.2f)); // 아주 부드럽게 퍼지게

    InitSpriteAnimation(1, 0.5f);

    SetPosition(pos);

    m_bReversePlay = true;

    return finalVelocity;
}

void AFeatherActor::Tick()
{
    AEffectActor::Tick(); // 기본 애니메이션(스프라이트 등)은 유지

    // 깃털 특유의 부드러운 낙하, 진동, 회전 등 커스텀 이동 적용
    Vec3 pos = GetPosition();

    // 예: 흔들리며 떨어지는 듯한 움직임
    float time = TIMER->GetGameTime(); // 전체 시간
    pos.x += sin(time * 3.0f) * 0.2f; // 좌우 흔들림

    SetPosition(pos);

    // (선택) 회전도 추가하고 싶다면:
    Vec3 rot = GetRotation();
    rot.z += 1.0f * TIMER->GetDeltaTime();
    SetRotation(rot);
}
