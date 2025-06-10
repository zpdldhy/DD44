#include "pch.h"
#include "WalkerMovement.h"

#include "AActor.h"
#include "TCharacter.h"
#include "EffectManager.h"

#include "Input.h"
#include "Timer.h"
#include "ObjectManager.h"

#include "PlayerMoveScript.h"


void WalkerMovement::Init()
{
	// position
	m_bWait = true;
	m_vPos[0] = GetOwner()->GetPosition() - m_half * Vec3(1, 0, 1);
	m_vPos[1] = GetOwner()->GetPosition() - m_half * Vec3(1, 0, -1);
	m_vPos[2] = GetOwner()->GetPosition() - m_half * Vec3(-1, 0, -1);
	m_vPos[3] = GetOwner()->GetPosition() - m_half * Vec3(-1, 0, 1);

	m_currentPosIdx = (int)RandomRange(0.0f, 3.0f);
	m_vCurrentTarget = m_vPos[m_currentPosIdx];
	m_vDistance = m_vCurrentTarget - GetOwner()->GetPosition();

	// ȸ�� 
	Vec3 direction = GetOwner()->GetPosition() - m_vCurrentTarget;
	direction.y = 0;
	direction.Normalize();
	Vec3 tempUp = { 0.0f, 1.0f, 0.0f };
	Vec3 moveDir = tempUp.Cross(direction); // �ݽð� ����
	float targetYaw = atan2f(moveDir.x, moveDir.z);
	Vec3 currentRot = GetOwner()->GetRotation();
	currentRot.y = targetYaw;
	GetOwner()->SetRotation(currentRot);

	// state
	idle = make_shared<WalkerIdleState>(GetOwner());
	walk = make_shared<WalkerWalkState>(GetOwner());
	hit = make_shared<WalkerHitState>(GetOwner());
	death = make_shared<WalkerDieState>(GetOwner());

	currentState = walk;
	currentState->Enter();

	// 
	dynamic_pointer_cast<TCharacter>(GetOwner())->SetHp(3);
}

void WalkerMovement::Tick()
{
	Flashing();
	LerpRotate();
	if (m_bWait)
	{
		m_wait -= TIMER->GetDeltaTime();
		if (m_wait < 0)
		{
			ChangeState(walk);
			m_bWait = false;
		}
		else
		{
			return;
		}
	}
	currentState->Tick();
	if (currentState->GetId() == ENEMY_STATE::ENEMY_S_DEATH)
	{
		if (!currentState->IsPlaying())
		{
			// ����
			GetOwner()->m_bDelete = true;
		}
		return;
	}
	if (currentState->GetId() == ENEMY_STATE::ENEMY_S_HIT)
	{
		if (!currentState->IsPlaying())
		{
			m_rotate = true;
			ChangeState(walk);
		}
		return;
	}

	// �ȴ� ��Ȳ�� �� ��ġ �����ذ��� �ȱ�
	if (currentState->GetId() == ENEMY_STATE::ENEMY_S_WALK && !m_bWait)
	{
		Vec3 pos = GetOwner()->GetPosition();
		Vec3 diff = m_vCurrentTarget - pos;
		if (diff.Length() < 0.1f)
		{
			// ���� ��ǥ ����
			m_currentPosIdx = (++m_currentPosIdx) % 4;
			m_vCurrentTarget = m_vPos[m_currentPosIdx];
			m_vDistance = m_vCurrentTarget - GetOwner()->GetPosition();

			// ȸ�� 
			Vec3 direction = GetOwner()->GetPosition() - m_vCurrentTarget;
			direction.y = 0;
			direction.Normalize();

			// direction�� �̿��� ȸ��
			Vec3 tempUp = { 0.0f, 1.0f, 0.0f };
			moveDir = tempUp.Cross(direction); // �ݽð� ����
			m_rotate = true;

			// ���� time ���
			m_bWait = true;
			m_wait = RandomRange(0.2f, 1.5f);
		}
		else
		{
			// �̵�
			Vec3 pos = m_vDistance * m_walkSpeed;
			GetOwner()->AddPosition(pos);
		}

	}
	// HIT
	CheckHit();

}

void WalkerMovement::ChangeState(shared_ptr<StateBase> _state)
{
	if (!currentState->IsInterruptible() && currentState->IsPlaying())
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
		currentState->Enter();
}

void WalkerMovement::PlayBloodBurst(const Vec3& _origin, const Vec3& _direction, float _speed, float _spreadAngleDeg, int _minCount, int _maxCount)
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
// �� �Լ�, ��� �ʿ��ϸ� �ֱ��.
void WalkerMovement::VisitAllMeshMaterials(shared_ptr<UMeshComponent> comp)
{
	if (!comp) return;

	shared_ptr<UMaterial> mat = comp->GetMaterial();
	if (mat)
	{

	}

	for (int i = 0; i < comp->GetChildCount(); ++i)
	{
		VisitAllMeshMaterials(comp->GetChild(i));
	}
}

void WalkerMovement::ApplyHitFlashToAllMaterials(shared_ptr<UMeshComponent> comp, float value)
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

void WalkerMovement::Flashing()
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

void WalkerMovement::LerpRotate()
{
	if (!m_rotate) return;
	float targetYaw = atan2f(moveDir.x, moveDir.z);
	Vec3 currentRot = GetOwner()->GetRotation();
	float currentYaw = currentRot.y;

	// ���� ���� ���
	float angleDiff = targetYaw - currentYaw;
	while (angleDiff > DD_PI)  angleDiff -= DD_PI * 2;
	while (angleDiff < -DD_PI) angleDiff += DD_PI * 2;

	// Lerp ���
	float smoothedYaw = currentRot.y + angleDiff * m_fRotationSpeed * TIMER->GetDeltaTime();

	currentRot.y = smoothedYaw;
	GetOwner()->SetRotation(currentRot);
}

void WalkerMovement::CheckHit()
{
	if (currentState->GetId() == ENEMY_STATE::ENEMY_S_DEATH)
	{
		return;
	}
	// ����ü �浹 Ȯ��
	auto healthComp = dynamic_pointer_cast<TCharacter>(GetOwner());
	// �浹 Ȯ��
	bool isCol = false;
	if (GetOwner()->m_vCollisionList.size() > 0)
	{
		// Melee ����
		auto list = GetOwner()->m_vCollisionList;
		for (auto& index : list)
		{
			if (OBJECT->GetActor(index.first)->m_szName == L"Melee")
				isCol = true;
		}
	}

	if (isCol || healthComp->IsHitByProjectile())
	{
		// Blood FX
		Vec3 basePos = GetOwner()->GetPosition();
		basePos.y += RandomRange(3, 4);
		Vec3 look = GetOwner()->GetLook();
		velocity = -look;
		PlayBloodBurst(basePos, velocity, 25.0f, 90.0f);

		m_fHitFlashTimer = 1.f;  // 1�� ����
		m_bIsFlashing = true;

		// Anim
		// ȸ�� 
		Vec3 direction = GetOwner()->GetPosition() - player.lock()->GetPosition();
		direction.y = 0;
		direction.Normalize();
		Vec3 tempUp = { 0.0f, 1.0f, 0.0f };
		Vec3 moveDir = tempUp.Cross(direction); // �ݽð� ����
		float targetYaw = atan2f(moveDir.x, moveDir.z);
		Vec3 currentRot = GetOwner()->GetRotation();
		currentRot.y = targetYaw;
		GetOwner()->SetRotation(currentRot);
		m_rotate = false;

		// Status
		auto healthComp = dynamic_pointer_cast<TCharacter>(GetOwner());
		healthComp->TakeDamage(1);
		if (healthComp->IsDead())
		{
			ChangeState(death);
			GetOwner()->AddPosition(Vec3(0.0f, -0.8f, 0.0f));
			return;
		}

		// Anim State
		ChangeState(hit);
	}
}
