#include "pch.h"
#include "EffectManager.h"
#include "ObjectManager.h"
#include "UStaticMeshComponent.h"
#include "UMaterial.h"
#include "CameraManager.h"
#include "ABloodActor.h"
#include "ADustActor.h"
#include "ParticleManager.h"
#include "AFeatherActor.h"
#include "AShockwaveActor.h"
#include "ABeamActor.h"
#include "APoppingDust.h"


void EffectManager::Init()
{
    for (int i = 0; i < 50; ++i)
    {
        for (int t = 0; t < (int)EEffectType::MAX; ++t)
        {
            EEffectType type = (EEffectType)t;
            auto actor = CreateEffectActor((EEffectType)t);
            actor->m_szName = L"Effect";
            m_mEffectPool[(EEffectType)t].push_back(actor);
            
            if (type == EEffectType::Dust || type == EEffectType::PoppingDust)
                PARTICLE->AddUI(actor);
            else
            {
                actor->Init();
                m_vEffectList.emplace_back(actor);
            }
        }
    }
}

void EffectManager::Tick()
{
    for(auto iter = m_vEffectList.begin();iter!= m_vEffectList.end();)
    {
        auto pEffect = *iter;
        if (pEffect->m_bDelete == true)
        {
            iter = m_vEffectList.erase(iter);
            continue;
        }

        pEffect->Tick();
        iter++;
    }
}

void EffectManager::Render()
{
    for(auto& pEffect : m_vEffectList)
    {
        if (pEffect->m_bRender==true)
        {
            pEffect->Render();
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
    case EEffectType::Feather:
        actor = make_shared<AFeatherActor>();
        break;
    case EEffectType::Shockwave:
        actor = make_shared<AShockwaveActor>();
        break;
    case EEffectType::Beam:
        actor = make_shared<ABeamActor>();
        break;
    case EEffectType::PoppingDust:
        actor = make_shared<APoppingDust>();
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
    case EEffectType::Feather:
        mat->Load(L"../Resources/Texture/feather.png", L"../Resources/Shader/SpriteUV.hlsl");
        break;
    case EEffectType::Shockwave:
        mat->Load(L"../Resources/Texture/circle.png", L"../Resources/Shader/SpriteWorld.hlsl");
        break;
    case EEffectType::Beam:
        mat->Load(L"../Resources/Texture/Wind.png", L"../Resources/Shader/SpriteWorld.hlsl");
        break;
    case EEffectType::PoppingDust:
        mat->Load(L"../Resources/Texture/smokeDustR_4x4.png", L"../Resources/Shader/SpriteUV.hlsl");
        break;
    }

    mesh->SetMaterial(mat);

    // 공통 설정
    actor->SetLoop(false);
    actor->SetAutoDestroy(false);
    actor->SetRender(false);

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


void EffectManager::PlayEffect(EEffectType type, const Vec3& pos, float maxAngleSpreadDeg, const Vec3& baseVelocity, float _scale)
{
    auto actor = GetReusableActor(type);
    if (!actor)
        return;

    
    float duration = 1.0f;

    switch (type)
    {
    case EEffectType::Dust:       duration = 1.0f; break;
    case EEffectType::Blood:      duration = 0.7f; break;
    case EEffectType::Shockwave:  
        duration = 0.8f;
        actor->SetRotation(Vec3(DD_PI/2, 0, 0));
        break;
    case EEffectType::Beam:       duration = 0.8; break;
    default:                      duration = 1.0f; break;
    }

    actor->Play(duration, actor->Prepare(pos, baseVelocity, _scale));
}

void EffectManager::PlayDustBurst(const Vec3& _origin, float _speed, float _size)
{
    static const Vec3 directions[8] =
    {
        Vec3(1.0f, 0.0f, 0.0f),    
        Vec3(-1.0f, 0.0f, 0.0f),   
        Vec3(0.0f, 0.0f, 1.0f),    
        Vec3(0.0f, 0.0f,-1.0f),    
        Vec3(0.0f, 0.5f, 0.0f),    
        Vec3(0.7071f, 0.0f, 0.7071f),   
        Vec3(-0.7071f, 0.0f, 0.7071f),  
        Vec3(0.0f, 0.0f, 0.0f)
    };

    for (int i = 0; i < 8; ++i)
    {
        Vec3 dir = directions[i];
        dir.y = 0.5f; 
        
        Vec3 velocity = dir * (_speed * 1.8f); 

        PlayEffect(EEffectType::PoppingDust, _origin, 0.f, velocity, _size);
    }
}

void EffectManager::PlayBeamBurst(const Vec3& origin, int count, float _scale)
{
    float angleStep = 2 * DD_PI / count;

    for (int i = 0; i < count; ++i)
    {
        float angle = i * angleStep;

        // 수평 방향 + 랜덤 위로 튐
        float randomY = RandomRange(0.3f, 0.7f); // 위쪽 분산 (0이면 바닥, 1이면 수직)
        Vec3 dir = Vec3(sinf(angle), randomY, cosf(angle));
        dir.Normalize();

        PlayEffect(EEffectType::Beam, origin, 0.0f, dir, _scale);
    }

  
}