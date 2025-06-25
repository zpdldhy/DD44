#include "pch.h"
#include "WalkerMovement.h"

#include "TEnemy.h"
#include "TPlayer.h"
#include "EffectManager.h"

#include "Input.h"
#include "Timer.h"
#include "ObjectManager.h"

#include "PlayerMoveScript.h"


void WalkerMovement::Init()
{
	auto player = dynamic_pointer_cast<TPlayer>(dynamic_pointer_cast<TEnemy>(GetOwner())->GetPlayer().lock());
	SetPlayer(player);

	dynamic_pointer_cast<TEnemy>(GetOwner())->SetHitOffset(0.5f);
	dynamic_pointer_cast<TEnemy>(GetOwner())->SetBloodPos(Vec2(2.0f, 3.0f));

	// position
	m_bWait = true;
	m_vPos[0] = GetOwner()->GetPosition() - m_half * Vec3(1, 0, 1);
	m_vPos[1] = GetOwner()->GetPosition() - m_half * Vec3(1, 0, -1);
	m_vPos[2] = GetOwner()->GetPosition() - m_half * Vec3(-1, 0, -1);
	m_vPos[3] = GetOwner()->GetPosition() - m_half * Vec3(-1, 0, 1);

	m_currentPosIdx = (int)RandomRange(0.0f, 3.0f);
	m_vCurrentTarget = m_vPos[m_currentPosIdx];
	m_vDistance = m_vCurrentTarget - GetOwner()->GetPosition();

	// 회전 
	Vec3 direction = GetOwner()->GetPosition() - m_vCurrentTarget;
	direction.y = 0;
	direction.Normalize();
	Vec3 tempUp = { 0.0f, 1.0f, 0.0f };
	Vec3 moveDir = tempUp.Cross(direction); // 반시계 방향
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
	dynamic_pointer_cast<TCharacter>(GetOwner())->SetSoul(3);
	dynamic_pointer_cast<WalkerDieState>(death)->SetPlayer(player);
}

void WalkerMovement::Tick()
{
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
			// 죽음
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

	// 걷는 상황일 때 위치 조절해가며 걷기
	if (currentState->GetId() == ENEMY_STATE::ENEMY_S_WALK && !m_bWait)
	{
		Vec3 pos = GetOwner()->GetPosition();
		Vec3 diff = m_vCurrentTarget - pos;
		if (diff.Length() < 0.1f)
		{
			// 다음 목표 설정
			m_currentPosIdx = (++m_currentPosIdx) % 4;
			m_vCurrentTarget = m_vPos[m_currentPosIdx];
			m_vDistance = m_vCurrentTarget - GetOwner()->GetPosition();

			// 회전 
			Vec3 direction = GetOwner()->GetPosition() - m_vCurrentTarget;
			direction.y = 0;
			direction.Normalize();

			// direction을 이용한 회전
			Vec3 tempUp = { 0.0f, 1.0f, 0.0f };
			moveDir = tempUp.Cross(direction); // 반시계 방향
			m_rotate = true;

			// 랜덤 time 대기
			m_bWait = true;
			m_wait = RandomRange(0.2f, 1.5f);
		}
		else
		{
			// 이동
			Vec3 pos = m_vDistance * m_walkSpeed;
			GetOwner()->AddPosition(pos);
		}

	}
	// HIT
	auto comp = dynamic_pointer_cast<TEnemy>(GetOwner());
	bool isHit = comp->CheckHit();
	if (isHit && !comp->IsDead())
	{

		ChangeState(hit);
	}
	if (comp->IsDead())
	{
		// 회전 
		Vec3 direction = GetOwner()->GetPosition() - player.lock()->GetPosition();
		direction.y = 0;
		direction.Normalize();
	
		Vec3 tempUp = { 0.0f, 1.0f, 0.0f };
		Vec3 moveDir = tempUp.Cross(direction); // 반시계 방향
		float targetYaw = atan2f(moveDir.x, moveDir.z);
		Vec3 currentRot = GetOwner()->GetRotation();
		currentRot.y = targetYaw;
		GetOwner()->SetRotation(currentRot);
		m_rotate = false;

		Vec3 playerPos = player.lock()->GetPosition();
		playerPos.y += 1.5f;
		Vec3 soulDirection = playerPos - GetOwner()->GetPosition();
		soulDirection.Normalize();
		EFFECT->PlayEffect(EEffectType::Soul, GetOwner()->GetPosition(), 0, soulDirection, 1.0f, playerPos);

		ChangeState(death);
	}

}

shared_ptr<UScriptComponent> WalkerMovement::Clone()
{
	auto script = make_shared<WalkerMovement>();
	return script;
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

void WalkerMovement::LerpRotate()
{
	if (!m_rotate) return;
	float targetYaw = atan2f(moveDir.x, moveDir.z);
	Vec3 currentRot = GetOwner()->GetRotation();
	float currentYaw = currentRot.y;

	// 각도 차이 계산
	float angleDiff = targetYaw - currentYaw;
	while (angleDiff > DD_PI)  angleDiff -= DD_PI * 2;
	while (angleDiff < -DD_PI) angleDiff += DD_PI * 2;

	// Lerp 계산
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
	// 투사체 충돌 확인
	auto healthComp = dynamic_pointer_cast<TCharacter>(GetOwner());
	// 충돌 확인
	bool isCol = false;
	if (GetOwner()->m_vCollisionList.size() > 0)
	{
		// Melee 인지
		auto list = GetOwner()->m_vCollisionList;
		for (auto& index : list)
		{
			if (OBJECT->GetActor(index.first)->m_szName == L"Melee")
				isCol = true;
		}
	}

	if (isCol || healthComp->IsHitByProjectile())
	{
		// Anim
		// 회전 
		Vec3 direction = GetOwner()->GetPosition() - player.lock()->GetPosition();
		direction.y = 0;
		direction.Normalize();
		Vec3 tempUp = { 0.0f, 1.0f, 0.0f };
		Vec3 moveDir = tempUp.Cross(direction); // 반시계 방향
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
