#include "pch.h"
#include "TCharacter.h"
#include "UScriptComponent.h"
#include "ProjectileManager.h"

void TCharacter::Tick()
{
	AActor::Tick();

	// hitByProjectile 값은 한 프레임 동안만 유지
	bHitByProjectile = false;

	////// 이 로직으론 바로 하위 자식만 적용됨
	//auto childMeshes = GetMeshComponent()->GetChildren();
	//for (auto child : childMeshes)
	//{
	//	if (child->GetType() == (int)MeshType::M_STATIC)
	//	{
	//		child->Tick();
	//	}
	//}

}

void TCharacter::Destroy()
{
	AActor::Destroy();
}

void TCharacter::SetHp(int _hp)
{
	hp = _hp;
	m_iMax_Hp = _hp;

	if (hp > 0)
		bDead = false;
}

void TCharacter::TakeDamage(int _damage)
{
	hp -= _damage;
	if (hp <= 0)
	{
		bDead = true;
	}
}

void TCharacter::CheckHitByProjectile(int type, bool _hit)
{
	if (type == (int)ProjectileType::MagicBall)
	{
		bHitByProjectile = _hit;
	}
}
