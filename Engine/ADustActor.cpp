#include "pch.h"
#include "ADustActor.h"
#include "CameraManager.h"

Vec3 ADustActor::Prepare(const Vec3& pos, const Vec3& baseVelocity)
{
    // ������ baseVelocity ������� �ణ ���� ȸ��
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

    // ���� gravity
    SetGravity(30.0f);

    // �۰� �����ؼ� �ε巴�� �����
    SetStartScale(Vec3(0.5f));
    SetEndScale(Vec3(1.2f)); // ���� �ε巴�� ������

    InitSpriteAnimation(4, 12.f);

    SetPosition(pos);

    return finalVelocity;
}
