#include "pch.h"
#include "ABloodActor.h"
#include "CameraManager.h"
#include "UStaticMeshComponent.h"
#include "ObjectManager.h"
#include "EffectManager.h"

Vec3 ABloodActor::Prepare(const Vec3& pos, const Vec3& baseVelocity, float _scale)
{
    float speed = baseVelocity.Length();
    if (speed < 0.001f)
        return Vec3(0, 0, 0);

    Vec3 baseDir = baseVelocity;
    baseDir.y = RandomRange(10.f, 30.f);
    baseDir.Normalize();

    // 1. 랜덤 각도 범위 설정 (좌우, 상하로 30도 정도)
    float spreadYaw = DegreesToRadians(RandomRange(-30.0f, 30.0f));   // 좌우 퍼짐
    float spreadPitch = DegreesToRadians(RandomRange(-20.0f, 20.0f));   // 위아래 퍼짐

    // 2. 회전 적용
    Matrix rotYaw = Matrix::CreateRotationY(spreadYaw);
    Matrix rotPitch = Matrix::CreateRotationX(spreadPitch);

    Vec3 dir = Vec3::TransformNormal(baseDir, rotYaw * rotPitch);
    dir.Normalize();

    Vec3 velocity = dir * speed * RandomRange(0.8f, 1.2f);
    Vec3 absDir = Vec3(fabs(dir.x), fabs(dir.y), fabs(dir.z)); // 방향성은 유지하되 scale로 쓰이게끔

    // 1. StartScale은 고정
    SetStartScale(Vec3(0.3f));

    // 2. EndScale은 방향 기반 + 랜덤 요소로 찢어짐 효과
    Vec3 endScale;
    endScale.x = Lerp(0.8f, 1.3f, RandomRange(0.0f, 1.0f)) + absDir.x * 2.0f;
    endScale.y = Lerp(0.8f, 1.3f, RandomRange(0.0f, 1.0f)) + absDir.y * 0.3f; // y는 작게
    endScale.z = Lerp(0.8f, 1.3f, RandomRange(0.0f, 1.0f)) + absDir.z * 2.0f;

    SetEndScale(endScale);
    SetPosition(pos);


    // 임시 수정 : 중력 및 이동
    m_pPhysics->SetWeight(0.5f);
    SetMove(velocity, 1.f);

    //return velocity;
    return Vec3();
}

void ABloodActor::Tick()
{
    for (auto& iter : m_vCollisionList)
    {
        auto pActor = OBJECT->GetActor(iter.first);
        if (!pActor) continue;

        if (pActor->m_szName == L"Terrain" )
        {
            Vec3 pos = iter.second.Inter;
            pos.y = pActor->GetPosition().y + 0.01f;
            // 디칼 찍기
            EFFECT->PlayEffect(EEffectType::BloodDecal, pos, 0.f, Vec3(0, 0, 0));

            // 바로 삭제
            m_bRender = false;
            break;
        }

        if (pActor->m_szName == L"Object")
        {
            Vec3 pos = iter.second.Inter;
            pos.y = pActor->GetPosition().y + 0.01f;
            // 디칼 찍기
            EFFECT->PlayEffect(EEffectType::BloodDecal, pos, 0.f, Vec3(0, 0, 0));

            // 바로 삭제
            m_bRender = false;
            break;
        }
    }

    AEffectActor::Tick();
}



