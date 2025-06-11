#include "pch.h"
#include "MageMovement.h"
#include "TCharacter.h"

#include "Timer.h"
#include "ObjectManager.h"

// temp temp temp !!!!!
#include "Input.h"

void MageMovement::Init()
{
	idle = make_shared<MageIdleState>(m_pOwner);
	appear = make_shared<MageAppearState>(m_pOwner);
	disappear = make_shared<MageDisappearState>(m_pOwner);
	hit = make_shared<MageHitState>(m_pOwner);
	attack = make_shared<MageAttackState>(m_pOwner);
	death = make_shared<MageDieState>(m_pOwner);
	ChangeState(idle);

	// HP
	dynamic_pointer_cast<TCharacter>(GetOwner())->SetHp(6);

	// Colllider


}

void MageMovement::Tick()
{
#pragma region AttackTest
	//if (INPUT->GetButton(B))
	//{
	//	ChangeState(appear);
	//}

	//if (INPUT->GetButton(M))
	//{
	//	ChangeState(disappear);
	//}


	//if (INPUT->GetButton(Z))
	//{
	//	dynamic_pointer_cast<MageHitState>(hit)->SetDirection(player.lock()->GetPosition());
	//	ChangeState(hit);
	//}

	//if (INPUT->GetButton(P))
	//{
	//	dynamic_pointer_cast<MageAttackState>(attack)->SetDirection(player.lock()->GetPosition());
	//	ChangeState(attack);
	//}

	//if (INPUT->GetButton(L))
	//{
	//	// ���� ���� ������ �� �� ������ ? 
	//	ChangeState(death);
	//}

#pragma endregion
	// FX
	Flashing();
	// State
	currentState->Tick();
	if (currentStateId == ENEMY_S_DEATH && !currentState->IsPlaying())
	{
		GetOwner()->m_bDelete = true;
		return;
	}

	if (!currentState->IsPlaying())
	{
		ChangeState(idle);
	}

	// Attack
	auto distance = player.lock()->GetPosition() - GetOwner()->GetPosition();
	if (distance.Length() < findDistance)
	{
		// ���� ���� Ȯ�� 
		if (currentStateId != ENEMY_S_ATTACK)
		{
			dynamic_pointer_cast<MageAttackState>(attack)->SetTarget(player);
			ChangeState(attack);
		}
	}

	//
	Rotate();

	// HP
	CheckHit();
}

void MageMovement::ChangeState(shared_ptr<StateBase> _state)
{
	// �Ʒ��� �ߺ��Ǵ� ���� �ƴϾ� ? ( Ȯ�� �ʿ� ) 
	if (_state->GetId() == ENEMY_S_DEATH)
	{
		if (currentState)
			currentState->End();

		currentState = _state;

		if (currentState)
		{
			currentState->Enter();
			currentStateId = currentState->GetId();
		}
		return;
	}

	if (currentState && !currentState->IsInterruptible() && currentState->IsPlaying())
	{
		if (!_state->GetId() == ENEMY_S_DEATH)
		{
			return;
		}
	}

	if (currentState)
		currentState->End();

	currentState = _state;


	if (currentState)
	{
		currentState->Enter();
		currentStateId = currentState->GetId();
	}
}

void MageMovement::Rotate()
{
	Vec3 dir = player.lock()->GetPosition() - m_pOwner.lock()->GetPosition();
	float targetYaw = atan2f(dir.x, dir.z);

	Vec3 currentRot = m_pOwner.lock()->GetRotation();
	float currentYaw = currentRot.y;

	float angleDiff = targetYaw - currentYaw;
	while (angleDiff > DD_PI)  angleDiff -= DD_PI * 2;
	while (angleDiff < -DD_PI) angleDiff += DD_PI * 2;

	float smoothedYaw = currentRot.y + angleDiff * 8.0f * TIMER->GetDeltaTime();

	currentRot.y = smoothedYaw;
	m_pOwner.lock()->SetRotation(currentRot);
}

void MageMovement::CheckHit()
{
	// ����ü �浹 Ȯ��
	auto healthComp = dynamic_pointer_cast<TCharacter>(GetOwner());

	hitElapsed += TIMER->GetDeltaTime();
	// �浹 Ȯ��
	if (hitElapsed > 1.0f) //&& GetOwner()->m_vCollisionList.size() > 0)
	{
		// ���� ���� Ȯ��
		bool isCol = false;
		if (GetOwner()->m_vCollisionList.size() > 0)
		{
			auto list = GetOwner()->m_vCollisionList;
			for (auto& index : list)
			{
				if (OBJECT->GetActor(index.first)->m_szName == L"Melee")
				{
					isCol = true;
				}
			}
		}

		if (isCol || healthComp->IsHitByProjectile())
		{
			hitElapsed = 0.0f;
			// FX
			m_fHitFlashTimer = 1.f;  // 1�� ����
			m_bIsFlashing = true;
			// HP
			healthComp->TakeDamage(1);
			// State
			// Anim
			if (healthComp->IsDead())
			{
				ChangeState(death);
			}
			else
			{
				// Attack�� �� ��¥�� ����. �߰� ���� �ʿ�
				//ChangeState(hit);
			}
		}
	}
}

void MageMovement::Flashing()
{
	if (m_bIsFlashing)
	{
		m_fHitFlashTimer -= TIMER->GetDeltaTime();
		if (m_fHitFlashTimer <= 0.0f)
		{
			m_fHitFlashTimer = 0.0f;
			m_bIsFlashing = false;
		}

		// hitFlashAmount�� 1 �� 0 ���� ����
		float hitFlashAmount = std::min(std::max<float>(m_fHitFlashTimer, 0.0f), 1.0f);

		auto root = GetOwner()->GetMeshComponent();
		ApplyHitFlashToAllMaterials(root, hitFlashAmount);
	}
}
void MageMovement::ApplyHitFlashToAllMaterials(shared_ptr<UMeshComponent> comp, float value)
{
	if (!comp) return;

	shared_ptr<UMaterial> mat = comp->GetMaterial();
	if (mat)
	{
		mat->SetHitFlashTime(value); // CB�� g_fHitFlashTime ����
	}

	for (int i = 0; i < comp->GetChildCount(); ++i)
	{
		ApplyHitFlashToAllMaterials(comp->GetChild(i), value);
	}
}