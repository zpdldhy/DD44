#include "pch.h"
#include "EffectManager.h"
#include "ObjectManager.h"
#include "UStaticMeshComponent.h"
#include "UMaterial.h"
#include "CameraManager.h"
#include "ABloodActor.h"
#include "ADustActor.h"


void EffectManager::Init()
{
    for (int i = 0; i < 20; ++i)
    {
        for (int t = 0; t <= (int)EEffectType::Dust; ++t)
        {
            auto actor = CreateEffectActor((EEffectType)t);
            actor->m_szName = L"Effect";
            m_mEffectPool[(EEffectType)t].push_back(actor);
            OBJECT->AddActor(actor);
        }
    }
}

shared_ptr<AEffectActor> EffectManager::CreateEffectActor(EEffectType type)
{
    shared_ptr<AEffectActor> actor;

    switch (type)
    {
    case EEffectType::Blood:
        actor = make_shared<ABloodActor>();
        break;
    case EEffectType::Dust:
        actor = make_shared<ADustActor>();
        break;
    }

    // 메시
    auto mesh = UStaticMeshComponent::CreatePlane();
    actor->SetMeshComponent(mesh);

    // 머티리얼
    auto mat = make_shared<UMaterial>();
    switch (type)
    {
    case EEffectType::Blood:
        mat->Load(L"../Resources/Texture/Blood2.png", L"../Resources/Shader/blood.hlsl");
        break;
    case EEffectType::Dust:
        mat->Load(L"../Resources/Texture/smokeDustR_4x4.png", L"../Resources/Shader/SpriteUV.hlsl");
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

    return nullptr;
}


void EffectManager::PlayEffect(EEffectType type, const Vec3& pos, float maxAngleSpreadDeg, const Vec3& baseVelocity)
{
    auto actor = GetReusableActor(type);
    if (!actor)
        return;

    actor->Play(0.7f, actor->Prepare(pos, baseVelocity));
}


