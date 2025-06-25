#include "pch.h"
#include "BatMovement.h"
#include "Timer.h"
#include <algorithm>
#include "EffectManager.h"
#include "UMeshComponent.h"
#include "TCharacter.h"

// temp
#include "Input.h"
#include "Sound.h"

// temp
#include "PlayerMoveScript.h"

#include "TPlayer.h"
// for collider
#include "TEnemy.h"
#include "UBoxComponent.h"
#include "ObjectManager.h"
#include "EnemyCollisionManager.h"

void BatMovement::Init()
{
	SetPlayer(dynamic_pointer_cast<TEnemy>(GetOwner())->GetPlayer());
	static_pointer_cast<TEnemy>(GetOwner())->SetBloodPos(Vec2(3, 4));

	m_vCenter = GetOwner()->GetPosition();

	// FSM
	idle = make_shared<BatIdleState>(m_pOwner);
	walk = make_shared<BatWalkState>(m_pOwner);
	attack = make_shared<BatAttackState>(m_pOwner);
	death = make_shared<BatDieState>(m_pOwner);

	currentState = idle;
	currentState->Enter();

	// Random movement
	m_bClockWise = (RandomRange(0, 10) > 5.0f) ? true : false;

	// Collider
	attackRangeActor = make_shared<AActor>();
	attackRangeActor->m_bCollision = false;

	auto collider = make_shared<UBoxComponent>();
	collider->m_bVisible = true;
	collider->SetName(L"Enemy");
	collider->SetLocalScale(Vec3(2.0f, 2.0f, 2.0f));
	collider->SetCollisionEnabled(CollisionEnabled::CE_QUERYONLY);
	attackRangeActor->SetShapeComponent(collider);

	colOffset = Vec3(1.0f, 1.0f, 1.0f);
	attackRangeActor->SetPosition(m_vCenter + colOffset * GetOwner()->GetLook() + Vec3(0.0f, 2.0f, 0.0f));
	attackRangeActor->m_szName = L"Enemy";

	OBJECT->AddActor(attackRangeActor);
	ENEMYCOLLIDER->Add(attackRangeActor);

	collider->m_bVisible = false;

	// Body
	GetOwner()->m_bCollision = true;

	// 상태값
	dynamic_pointer_cast<TCharacter>(GetOwner())->SetHp(1);
	dynamic_pointer_cast<TCharacter>(GetOwner())->SetSoul(2);
	dynamic_pointer_cast<BatDieState>(death)->SetPlayer(player);
}


void BatMovement::Tick()
{
	auto pos = GetOwner()->GetPosition();
	attackRangeActor->SetPosition(pos + colOffset * GetOwner()->GetLook() + Vec3(0.0f, 2.0f, 0.0f));

	currentState->Tick();
	if (currentState->GetId() == ENEMY_STATE::ENEMY_S_DEATH)
	{
		if (!currentState->IsPlaying())
		{
			// bat 죽음
			GetOwner()->m_bDelete = true;
			/*attackRangeActor->m_bDelete = true;*/

		}
		return;
	}

	// HIT
	CheckHit();

	if (m_bReturn)
	{
		ReturningToPos();
		return;
	}
	if (currentState->GetId() != ENEMY_STATE::ENEMY_S_ATTACK && currentState->GetId() != ENEMY_STATE::ENEMY_S_DEATH)
	{
		float deltaTime = TIMER->GetDeltaTime();
		{
			// 획일성을 줄이기 위해 
			rotateSpeed = RandomRange((originRotateSpeed - 0.3f), (originRotateSpeed + 0.5f));

		}
		if (m_bClockWise)
		{
			angle -= rotateSpeed * deltaTime; // 시계
		}
		else
		{
			angle += rotateSpeed * deltaTime; // 반시계
		}

		// 2PI 넘으면 되돌림
		if (angle > 2 * DD_PI) angle -= 2 * DD_PI;

		// 위치 갱신
		Vec3 pos;
		pos.x = m_vCenter.x + m_fRadius * std::cos(angle);
		pos.z = m_vCenter.z + m_fRadius * std::sin(angle);

		GetOwner()->SetPosition(pos);

		Vec3 direction = m_vCenter - GetOwner()->GetPosition();
		direction.y = 0;
		direction.Normalize();

		// direction을 이용한 회전
		Vec3 tempUp = { 0.0f, 1.0f, 0.0f };
		Vec3 moveDir;
		if (m_bClockWise)
		{
			moveDir = direction.Cross(tempUp); // 시계 방향
		}
		else
		{
			moveDir = tempUp.Cross(direction); // 반시계 방향
		}

		// 회전 TargetPos 바라보기
		float targetYaw = atan2f(moveDir.x, moveDir.z);
		Vec3 currentRot = GetOwner()->GetRotation();
		currentRot.y = targetYaw;
		GetOwner()->SetRotation(currentRot);

		//
		Vec3 distance = player.lock()->GetPosition() - GetOwner()->GetPosition();

		if (distance.Length() < 5.0f && distance.Length() > 3.0f)
		{
			m_bCanStartAttack = true;
			m_vTargetPos = player.lock()->GetPosition();
		}

		Attack();
	}
	else
	{
		// 공격에 맞춘 이동
		m_fLerp += m_fLerpOffset;
		Vec3 diff = m_vTargetPos - GetOwner()->GetPosition();
		Vec3 pos = diff * m_fLerp;

		GetOwner()->AddPosition(pos);

		if (currentState->GetId() == ENEMY_S_ATTACK && !currentState->IsPlaying())
		{
			// ReturnPos 찾기
			// 시도 1 : 가장 가까운 원 위의 한 점으로 이동
			{
				Vec3 dir = m_vCenter - GetOwner()->GetPosition();
				Vec3 rDir = dir;
				rDir.Normalize();
				Vec3 temp = rDir * m_fRadius;

				Vec3 diff = dir - temp;
				m_vReturnPos = GetOwner()->GetPosition() + diff;
			}
			// 시도 2 : 다른 접점으로 이동
			{
				//Vec3 d = GetOwner()->GetPosition() - m_vCenter;
				//float dLength = d.Length();
				//Vec3 d_norm = d / dLength;

				//Vec3 tempUp = Vec3(0, 1, 0);
				//Vec3 t1 = d_norm.Cross(tempUp);

				//m_vReturnPos = m_vCenter + t1 * m_fRadius;
			}

			// 
			m_bReturn = true;
			ChangetState(walk);
			m_fLerp = -0.05;
		}
	}


}

shared_ptr<UScriptComponent> BatMovement::Clone()
{
	auto script = make_shared<BatMovement>();
	return script;
}

void BatMovement::ChangetState(shared_ptr<StateBase> _state)
{
	if (_state->GetId() == ENEMY_S_DEATH)
	{
		// 죽었을때 콜라이더 처리
		attackRangeActor->m_bDelete = true;
		attackRangeActor->m_bCollision = false;

		if (currentState)
			currentState->End();

		currentState = _state;

		if (currentState)
			currentState->Enter();
	}
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

void BatMovement::ReturningToPos()
{
	Vec3 ownerPos = GetOwner()->GetPosition();
	Vec3 diff = m_vReturnPos - ownerPos;
	if (abs(diff.x) <= 0.7f || abs(diff.z) <= 0.7f)
	{
		attackRangeActor->m_bCollision = false;
		attackRangeActor->GetShapeComponent()->m_bVisible = false;
		m_bReturn = false;
	}

	Vec3 pos = diff * 0.005f;
	GetOwner()->AddPosition(pos);

	Vec3 currentPos = GetOwner()->GetPosition();
	Vec3 dir = currentPos - m_vCenter;
	dir.y = 0; // y는 무시

	angle = std::atan2(dir.z, dir.x);

	// 회전 TargetPos 바라보기
	Vec3 lookDir = m_vReturnPos - ownerPos;
	float targetYaw = atan2f(lookDir.x, lookDir.z);
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

void BatMovement::Attack()
{
	if (m_bCanStartAttack) 
	{
		attackRangeActor->m_bCollision = true;
		attackRangeActor->GetShapeComponent()->m_bVisible = true;
		// 이 부분 처리 개선 필요
		if (!currentState->IsInterruptible() && currentState->IsPlaying())
		{
			return;
		}
		m_vPrevPos = GetOwner()->GetPosition();
		//m_vTargetPos = m_vPrevPos + GetOwner()->GetLook() * 5.0f;
		Vec3 diff = m_vTargetPos - GetOwner()->GetPosition();

		// 회전 TargetPos 바라보기
		float targetYaw = atan2f(diff.x, diff.z);
		Vec3 currentRot = GetOwner()->GetRotation();
		currentRot.y = targetYaw;
		GetOwner()->SetRotation(currentRot);

		ChangetState(attack);
		m_bCanStartAttack = false;
	}
}

void BatMovement::CheckHit()
{
	auto comp = dynamic_pointer_cast<TEnemy>(GetOwner());
	bool isHit = comp->CheckHit();
	if (isHit && !comp->IsDead())
	{
		// 한대맞으면 바로 죽음
		//ChangetState(hit);
	}
	if (comp->IsDead())
	{
		ChangetState(death);
	}
}

//void BatMovement::CheckHit()
//{
//	if (currentState->GetId() != ENEMY_STATE::ENEMY_S_DEATH)
//	{
//		// 투사체 충돌 확인
//		auto healthComp = dynamic_pointer_cast<TCharacter>(GetOwner());
//
//		// 충돌 확인
//		bool isCol = false;
//		if (GetOwner()->m_vCollisionList.size() > 0)
//		{
//			// Melee 인지
//			auto list = GetOwner()->m_vCollisionList;
//			for (auto& index : list)
//			{
//				if (OBJECT->GetActor(index.first)->m_szName == L"Melee")
//					isCol = true;
//			}
//		}
//		if (isCol || healthComp->IsHitByProjectile())
//		{
//			// Anim
//			ChangetState(death);
//		}
//	}
//}
