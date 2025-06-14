#include "pch.h"
#include "TEnemy.h"
#include "TPlayer.h"
#include "Timer.h"
#include "ObjectManager.h"
#include "EffectManager.h"

void TEnemy::Tick()
{
	TCharacter::Tick();
	
	// FX
	Flashing();
	ApplyCrash();
}

bool TEnemy::CheckHit()
{
	hitElapsed += TIMER->GetDeltaTime();
	// 충돌 확인
	if (hitElapsed > 1.0f)
	{
		// 근접 공격 확인
		bool isCol = false;
		if (m_vCollisionList.size() > 0)
		{
			for (auto& index : m_vCollisionList)
			{
				if (OBJECT->GetActor(index.first)->m_szName == L"Melee")
				{
					isCol = true;
				}
			}
		}

		if (isCol || IsHitByProjectile())
		{
			hitElapsed = 0.0f;
			// FX
			//// Flashing
			m_fHitFlashTimer = 1.f;  // 1초 동안
			m_bIsFlashing = true;
			//// Blood
			Vec3 basePos = GetPosition();
			basePos.y += RandomRange(3, 4);
			Vec3 look = GetLook();
			velocity = -look;
			PlayBloodBurst(basePos, velocity, 25.0f, 90.0f);
			// HP
			TakeDamage(1);
			dynamic_pointer_cast<TPlayer>(player.lock())->IncArrowCount();

			return true;
		}
	}
	return false;
}

void TEnemy::Flashing()
{
	if (m_bIsFlashing)
	{
		m_fHitFlashTimer -= TIMER->GetDeltaTime();
		if (m_fHitFlashTimer <= 0.0f)
		{
			m_fHitFlashTimer = 0.0f;
			m_bIsFlashing = false;
		}

		// hitFlashAmount는 1 → 0 으로 감소
		float hitFlashAmount = std::min(std::max<float>(m_fHitFlashTimer, 0.0f), 1.0f);

		auto root = GetMeshComponent();
		ApplyHitFlashToAllMaterials(root, hitFlashAmount);
	}
}
void TEnemy::ApplyHitFlashToAllMaterials(shared_ptr<UMeshComponent> comp, float value)
{
	if (!comp) return;

	shared_ptr<UMaterial> mat = comp->GetMaterial();
	if (mat)
	{
		mat->SetHitFlashTime(value); // CB에 g_fHitFlashTime 전달
	}

	for (int i = 0; i < comp->GetChildCount(); ++i)
	{
		ApplyHitFlashToAllMaterials(comp->GetChild(i), value);
	}
}
void TEnemy::ApplyCrash()
{
	if (!m_bCrashSet && GetHp() <= (GetMaxHp() * 0.5f))
	{
		m_bCrashSet = true;

		auto root = GetMeshComponent();
		ApplyCrashToAllMaterials(root, true);
	}
}
void TEnemy::PlayBloodBurst(const Vec3& _origin, const Vec3& _direction, float _speed, float _spreadAngleDeg, int _minCount, int _maxCount)
{
	int count = RandomRange(_minCount, _maxCount);
	for (int i = 0; i < count; ++i)
	{
		Vec3 offset = Vec3(RandomRange(-0.3f, 0.3f), RandomRange(-0.3f, 0.3f), RandomRange(-0.3f, 0.3f));
		Vec3 pos = _origin + offset;

		Vec3 baseVelocity = _direction * _speed;
		EFFECT->PlayEffect(EEffectType::Blood, pos, _spreadAngleDeg, baseVelocity);
	}
}

void TEnemy::ApplyCrashToAllMaterials(shared_ptr<UMeshComponent> comp, bool enabled)
{
	if (!comp) return;

	shared_ptr<UMaterial> mat = comp->GetMaterial();
	if (mat)
		mat->SetCrash(enabled); // 내부에서 PushData()

	for (int i = 0; i < comp->GetChildCount(); ++i)
	{
		ApplyCrashToAllMaterials(comp->GetChild(i), enabled);
	}
}