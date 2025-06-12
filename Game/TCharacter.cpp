#include "pch.h"
#include "TCharacter.h"
#include "UScriptComponent.h"

void TCharacter::Tick()
{
	AActor::Tick();

	// hitByProjectile ���� �� ������ ���ȸ� ����
	bHitByProjectile = false;

	//// �� �������� �ٷ� ���� �ڽĸ� �����
	auto childMeshes = GetMeshComponent()->GetChildren();
	for (auto child : childMeshes)
	{
		if (child->GetType() == (int)MeshType::M_STATIC)
		{
			child->Tick();
		}
	}

}

void TCharacter::SetHp(int _hp)
{
	hp = _hp;
	m_iMax_Hp = _hp;
}

void TCharacter::TakeDamage(int _damage)
{
	hp -= _damage;
	if (hp <= 0)
	{
		bDead = true;
	}
}
