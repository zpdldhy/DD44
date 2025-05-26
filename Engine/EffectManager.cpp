#include "pch.h"
#include "EffectManager.h"
#include "ObjectManager.h"
#include "UStaticMeshComponent.h"
#include "UMaterial.h"


void EffectManager::Init()
{
    for (int i = 0; i < 50; ++i) 
    {
        for (int t = 0; t <= (int)EEffectType::Dust; ++t)
        {
            auto actor = CreateEffectActor((EEffectType)t);
            m_mEffectPool[(EEffectType)t].push_back(actor);
            OBJECT->AddActor(actor); 
        }
    }
}

shared_ptr<AEffectActor> EffectManager::CreateEffectActor(EEffectType type)
{
    auto actor = make_shared<AEffectActor>(type);

    // 메시
    auto mesh = UStaticMeshComponent::CreatePlane();
    actor->SetMeshComponent(mesh);

    // 머티리얼
    auto mat = make_shared<UMaterial>();
    switch (type)
    {
    case EEffectType::Blood:
        mat->Load(L"../Resources/Texture/Blood.png", L"../Resources/Shader/SpriteUV.hlsl");
        break;
    case EEffectType::Dust:
        mat->Load(L"../Resources/Texture/Dust.png", L"../Resources/Shader/SpriteUV.hlsl");
        break;
    }

    mesh->SetMaterial(mat);

    // 공통 설정
    actor->SetLoop(false);
    actor->SetAutoDestroy(false);
    actor->SetRender(false);
    actor->SetDuration(0.5f);

    return actor;
}

shared_ptr<AEffectActor> EffectManager::GetReusableActor(EEffectType type)
{
    for (auto& actor : m_mEffectPool[type])
    {
        if (!actor->IsActive())
            return actor;
    }

    //// 다 쓰였으면 새로 생성
    //auto newActor = CreateEffectActor(type);
    //m_mEffectPool[type].push_back(newActor);
    //OBJECT->AddActor(newActor);
    return nullptr;
}

void EffectManager::PlayEffect(EEffectType type, const Vec3& pos, float maxAngleSpreadDeg, const Vec3& baseVelocity)
{
    auto actor = GetReusableActor(type);
    if (!actor)
        return;

    float randomAngleDeg = RandomRange(-maxAngleSpreadDeg, maxAngleSpreadDeg);
    float rad = DegreesToRadians(randomAngleDeg);

    // baseVelocity 회전
    Vec3 velocity;
    velocity.x = baseVelocity.x * cos(rad) - baseVelocity.y * sin(rad);
    velocity.y = baseVelocity.x * sin(rad) + baseVelocity.y * cos(rad);
    velocity.z = baseVelocity.z;

    // 방향에 맞는 각도 계산
    float visualAngle = atan2(velocity.y, velocity.x) * (180.0f / 3.14159265f);

    actor->SetPosition(pos);
    actor->SetRotation(Vec3(0, 0, visualAngle)); // sprite 회전 적용
    actor->Play(1.0f, velocity);
}

void EffectManager::Tick()
{
    for (auto& pair : m_mEffectPool)
    {
        auto& list = pair.second;
        for (auto& actor : list)
            actor->Tick();
    }
}

void EffectManager::Render()
{
    for (auto& pair : m_mEffectPool)
    {
        auto& list = pair.second;
        for (auto& actor : list)
            actor->Render();
    }
}

void EffectManager::Destroy()
{
    for (auto& pair : m_mEffectPool)
    {
        pair.second.clear();
    }
    m_mEffectPool.clear();
}