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
#include "ABloodDecalActor.h"
#include "AInstance.h"
#include "USphereComponent.h"
#include "RenderStateManager.h"
#include "APointEffectActor.h"
#include "ASoulActor.h"

constexpr int INITIAL_POOL_SIZE = 0;

void EffectManager::Init()
{
    for (size_t index = 0; index < static_cast<size_t>(EEffectType::Count); ++index)
    {
        m_vInstanceEffect[index] = make_shared<AInstance>();
        m_vInstanceEffect[index]->SetEffectType(static_cast<EEffectType>(index));
    }

	for (int i = 0; i < INITIAL_POOL_SIZE; ++i)
	{
		for (int t = 0; t < static_cast<size_t>(EEffectType::Count); ++t)
		{
			EEffectType type = static_cast<EEffectType>(t);
			auto actor = CreateEffectActor(type);
			m_mEffectPool[type].push_back(actor);

			if (type == EEffectType::Dust || type == EEffectType::PoppingDust)
				PARTICLE->AddUI(actor);
			else
			{
				actor->Init();
				m_vEffectList.emplace_back(actor);
			}
		}
	}
	PlayInit();
}

void EffectManager::Tick()
{
    // 복사본 리스트 생성
    vector<shared_ptr<AEffectActor>> currentList = m_vEffectList;

    for (auto& pEffect : currentList)
    {
        if (pEffect->m_bDelete)
        {
            auto iter = std::find(m_vEffectList.begin(), m_vEffectList.end(), pEffect);
            if (iter != m_vEffectList.end())
                m_vEffectList.erase(iter);
            continue;
        }

        pEffect->Tick();
    }
}

void EffectManager::Render()
{
    for(auto& pEffect : m_vInstanceEffect)
    {
        if (pEffect == nullptr) continue;

        if (pEffect->m_bRender==true)
        {
            if(pEffect->GetEffectType() == EEffectType::BloodDecal)
            {
                RenderOption opt;
                opt.blend = BlendType::AlphaBlend;
                opt.depth = DepthType::ZTestOn_ZWriteOff;
                opt.cull = CullType::None;
                opt.rasterizer = RasterizerType::SolidNone;
                STATEMANAGER->Apply(opt);
            }
            pEffect->Render();

            if (pEffect->GetEffectType() == EEffectType::BloodDecal)
            {
                STATEMANAGER->Restore();
            }

        }
	}
}

shared_ptr<AEffectActor> EffectManager::CreateEffectActor(EEffectType type)
{
    shared_ptr<AEffectActor> actor;
    auto collider = make_shared<USphereComponent>();

    switch (type)
    {
    case EEffectType::Blood:
        actor = make_shared<ABloodActor>();
        collider->SetName(L"Blood");
        collider->SetLocalScale(Vec3(.1f, .1f, .1f));
        collider->SetCollisionEnabled(CollisionEnabled::CE_QUERYONLY);
        actor->SetShapeComponent(collider);

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
    case EEffectType::BloodDecal:
        actor = make_shared<ABloodDecalActor>();
        break;
    case EEffectType::Point:
        actor = make_shared<APointEffectActor>();
        break;
    case EEffectType::Soul:
        actor = make_shared<ASoulActor>();
        collider->SetName(L"Soul");
        collider->SetLocalScale(Vec3(.1f, .1f, .1f));
        collider->SetCollisionEnabled(CollisionEnabled::CE_QUERYONLY);
        actor->SetShapeComponent(collider);
        actor->m_bAutoScale = false;
        break;
    }

    //그림자
    actor->m_bCastShadow = false;

	// 메시
	auto mesh = UStaticMeshComponent::CreatePlane();
	actor->SetMeshComponent(mesh);

    // 머티리얼
    auto mat = make_shared<UMaterial>();
    switch (type)
    {
    case EEffectType::Blood:
        mat->Load(L"../Resources/Texture/Blood2.png", L"../Resources/Shader/blood.hlsl");
        m_vInstanceEffect[static_cast<size_t>(EEffectType::Blood)]->AddInstanceMesh(mesh);
        actor->m_szName = L"BloodEffect";
        break;
    case EEffectType::Dust:
        mat->Load(L"../Resources/Texture/smokeDustR_4x4.png", L"../Resources/Shader/SpriteUV.hlsl");
        actor->m_szName = L"DustEffect";
       // m_vInstanceEffect[static_cast<size_t>(EEffectType::Dust)]->AddInstanceMesh(mesh);
        break;
    case EEffectType::Feather:
        mat->Load(L"../Resources/Texture/feather.png", L"../Resources/Shader/SpriteUV.hlsl");
        m_vInstanceEffect[static_cast<size_t>(EEffectType::Feather)]->AddInstanceMesh(mesh);
        actor->m_szName = L"FeatherEffect";
        break;
    case EEffectType::Shockwave:
        mat->Load(L"../Resources/Texture/circle.png", L"../Resources/Shader/SpriteWorld.hlsl");
        m_vInstanceEffect[static_cast<size_t>(EEffectType::Shockwave)]->AddInstanceMesh(mesh);
        actor->m_szName = L"ShockEffect";
        break;
    case EEffectType::Beam:
        mat->Load(L"../Resources/Texture/Wind.png", L"../Resources/Shader/SpriteWorld.hlsl");
        m_vInstanceEffect[static_cast<size_t>(EEffectType::Beam)]->AddInstanceMesh(mesh);
        actor->m_szName = L"BeamEffect";
        break;
    case EEffectType::PoppingDust:
        mat->Load(L"../Resources/Texture/smokeDustR_4x4.png", L"../Resources/Shader/SpriteUV.hlsl");
        actor->m_szName = L"PopDustEffect";
       // m_vInstanceEffect[static_cast<size_t>(EEffectType::PoppingDust)]->AddInstanceMesh(mesh);
        break;
    case EEffectType::BloodDecal:
        mat->Load(L"../Resources/Texture/bloodspatter.png", L"../Resources/Shader/SpriteWorld.hlsl");
        m_vInstanceEffect[static_cast<size_t>(EEffectType::BloodDecal)]->AddInstanceMesh(mesh);
        actor->m_szName = L"DecalEffect";
        break;
    case EEffectType::Point:
        mat->Load(L"../Resources/Texture/spark_particle.png", L"../Resources/Shader/SpriteUV.hlsl");
        m_vInstanceEffect[static_cast<size_t>(EEffectType::Point)]->AddInstanceMesh(mesh);
        actor->m_szName = L"PointEffect";
        break;
    case EEffectType::Soul:
        mat->Load(L"../Resources/Texture/Soft.png", L"../Resources/Shader/SpriteUV.hlsl");
        m_vInstanceEffect[static_cast<size_t>(EEffectType::Soul)]->AddInstanceMesh(mesh);
        actor->m_szName = L"PointEffect";
        break;
    }

	mesh->SetMaterial(mat);

	// 공통 설정
	actor->m_bLoop = false;
	actor->m_bAutoDestroy = false;
	actor->m_bRender = false;

	return actor;
}

shared_ptr<AEffectActor> EffectManager::GetReusableActor(EEffectType type)
{
	for (auto& actor : m_mEffectPool[type])
	{
		if (!actor->IsActive())
			return actor;
	}

	auto newActor = CreateEffectActor(type);
	newActor->Init();
	m_mEffectPool[type].push_back(newActor);

	// UI 전용이면 등록
	if (type == EEffectType::Dust || type == EEffectType::PoppingDust)
		PARTICLE->AddUI(newActor);
	else
 		m_vEffectList.emplace_back(newActor);

	return newActor;
}


void EffectManager::PlayEffect(EEffectType type, const Vec3& pos, float maxAngleSpreadDeg, const Vec3& baseVelocity, float _scale, const Vec3& targetPos)
{
	auto actor = GetReusableActor(type);
	if (!actor)
		return;

    
    float duration = 1.0f;
    actor->GetMeshComponent()->SetInstanceColor(Vec4(1.f, 1.f, 1.f, 1.f));
    float angle = RandomRange(0.0f, DD_2PI);
    auto pSoul = dynamic_pointer_cast<ASoulActor>(actor);
    switch (type)
    {
    case EEffectType::Dust:       duration = 1.0f; break;
    case EEffectType::Blood:      duration = 0.7f; break;
    case EEffectType::Shockwave:  
        duration = 0.8f;
        actor->SetRotation(Vec3(DD_PI/2, 0, 0));
        actor->GetMeshComponent()->SetInstanceColor(Vec4(1, 1, 1, 1));
        break;
    case EEffectType::Beam: 
        duration = 0.8f;
        actor->GetMeshComponent()->SetInstanceColor(Vec4(1, 1, 1, 1));
        break;
    case EEffectType::BloodDecal: 
        duration = 10.0f;
        actor->SetRotation(Vec3(DD_PI / 2, 0, angle));
        actor->GetMeshComponent()->SetInstanceColor(Vec4(0.6f, 0.05f, 0.05f, 1.f));        
        break;
    case EEffectType::Point:
        duration = 0.2f;
        actor->GetMeshComponent()->SetInstanceColor(Vec4(0.7f, 0.9f, 0.95f, 1.0f));
        break;
    case EEffectType::Soul:
        if (pSoul)
        {
            pSoul->SetTarget(targetPos);
        }
        duration = 10.f;
        break;
    default:                      duration = 1.0f; break;
    }

    actor->Play(duration, actor->Prepare(pos, baseVelocity, _scale));

    if (type == EEffectType::Blood)
    {
        m_vBloodActorList.push_back(actor); 
    }
    else if (type == EEffectType::Soul)
    {
        m_vSoulActorList.push_back(actor);
    }
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

void EffectManager::EffectMove()
{
    for (auto pEffect : m_vEffectList)
    {
        pEffect->GetPhysicsComponent()->Tick();
        if (pEffect->GetShapeComponent())
        {
            pEffect->GetShapeComponent()->UpdateMatrix();
            pEffect->GetShapeComponent()->UpdateBounds();
        }
    }
}



void EffectManager::PlayInit()
{
	for (int i = 0; i < 3; ++i)
	{
		PlayEffect(EEffectType::Feather, Vec3(0, 0, 0), 0.0f, Vec3(0, 0, 0));
     
	}
    for (int i = 0; i < 10; ++i)
    {
        PlayEffect(EEffectType::BloodDecal, Vec3(0, 0, 0), 0.0f, Vec3(0, 0, 0));
        PlayEffect(EEffectType::Soul, Vec3(0, 0, 0), 0.0f, Vec3(0, 0, 0));
    }

	PlayEffect(EEffectType::Dust, Vec3(0, 0, 0), 0.0f, Vec3(0, 0, 0));
	PlayEffect(EEffectType::Point, Vec3(0, 0, 0), 0.0f, Vec3(0, 0, 0));

}
