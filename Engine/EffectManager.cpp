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
#include "AInstance.h"
#include "APointEffectActor.h"

constexpr int INITIAL_POOL_SIZE = 3;

void EffectManager::Init()
{
	for (size_t index = 0; index < static_cast<size_t>(EEffectType::Count); ++index)
	{
		m_vInstanceEffect[index] = make_shared<AInstance>();
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
	for (auto iter = m_vEffectList.begin(); iter != m_vEffectList.end();)
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
	for (auto& pEffect : m_vInstanceEffect)
	{
		if (pEffect->m_bRender == true)
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
	case EEffectType::Point:
		actor = make_shared<APointEffectActor>();
		break;
	}

	//그림자
	actor->m_szName = L"Effect";
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
		break;
	case EEffectType::Dust:
		mat->Load(L"../Resources/Texture/smokeDustR_4x4.png", L"../Resources/Shader/SpriteUV.hlsl");
		// m_vInstanceEffect[static_cast<size_t>(EEffectType::Dust)]->AddInstanceMesh(mesh);
		break;
	case EEffectType::Feather:
		mat->Load(L"../Resources/Texture/feather.png", L"../Resources/Shader/SpriteUV.hlsl");
		m_vInstanceEffect[static_cast<size_t>(EEffectType::Feather)]->AddInstanceMesh(mesh);
		break;
	case EEffectType::Shockwave:
		mat->Load(L"../Resources/Texture/circle.png", L"../Resources/Shader/SpriteWorld.hlsl");
		m_vInstanceEffect[static_cast<size_t>(EEffectType::Shockwave)]->AddInstanceMesh(mesh);
		break;
	case EEffectType::Beam:
		mat->Load(L"../Resources/Texture/Wind.png", L"../Resources/Shader/SpriteWorld.hlsl");
		m_vInstanceEffect[static_cast<size_t>(EEffectType::Beam)]->AddInstanceMesh(mesh);
		break;
	case EEffectType::PoppingDust:
		mat->Load(L"../Resources/Texture/smokeDustR_4x4.png", L"../Resources/Shader/SpriteUV.hlsl");
		// m_vInstanceEffect[static_cast<size_t>(EEffectType::PoppingDust)]->AddInstanceMesh(mesh);
		break;
	case EEffectType::Point:
		mat->Load(L"../Resources/Texture/spark_particle.png", L"../Resources/Shader/SpriteUV.hlsl");
		m_vInstanceEffect[static_cast<size_t>(EEffectType::Point)]->AddInstanceMesh(mesh);
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


void EffectManager::PlayEffect(EEffectType type, const Vec3& pos, float maxAngleSpreadDeg, const Vec3& baseVelocity, float _scale)
{
	auto actor = GetReusableActor(type);
	if (!actor)
		return;


	float duration = 1.0f;
	actor->GetMeshComponent()->SetInstanceColor(Vec4(1.f, 1.f, 1.f, 1.f));
	
	switch (type)
	{
	case EEffectType::Dust:       duration = 1.0f; break;
	case EEffectType::Blood:      duration = 0.7f; break;
	case EEffectType::Shockwave:
		duration = 0.8f;
		actor->SetRotation(Vec3(DD_PI / 2, 0, 0));
		break;
	case EEffectType::Beam:       duration = 0.8; 
		break;
	case EEffectType::Point:
		duration = 0.2;
		actor->GetMeshComponent()->SetInstanceColor(Vec4(0.7f, 0.9f, 0.95, 1.0f));
		break;
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

void EffectManager::PlayInit()
{
	for (int i = 0; i < INITIAL_POOL_SIZE; ++i)
	{
		PlayEffect(EEffectType::Feather, Vec3(0, 0, 0), 0.0f, Vec3(0, 0, 0));
	}
	PlayEffect(EEffectType::Dust, Vec3(0, 0, 0), 0.0f, Vec3(0, 0, 0));
	PlayEffect(EEffectType::Point, Vec3(0, 0, 0), 0.0f, Vec3(0, 0, 0));
}
