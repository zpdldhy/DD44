#include "pch.h"
#include "HeadRollerMovement.h"
#include "TPlayer.h"
#include "TEnemy.h"
#include "Timer.h"
#include "EffectManager.h"

// temptemptemptmepte
#include "Input.h"

void HeadRollerMovement::Init()
{
	auto player = dynamic_pointer_cast<TPlayer>(dynamic_pointer_cast<TEnemy>(GetOwner())->GetPlayer().lock());
	SetPlayer(player);

	// State
	idle = make_shared<HeadRollerIdleState>(GetOwner());
	look = make_shared<HeadRollerLookState>(GetOwner());
	roll = make_shared<HeadRollerAttackState>(GetOwner());
	walk = make_shared<WalkerWalkState>(GetOwner());
	hit = make_shared<WalkerHitState>(GetOwner());
	death = make_shared<HeadRollerDieState>(GetOwner());
	ChangeState(idle);

	// HP
	dynamic_pointer_cast<TCharacter>(GetOwner())->SetHp(3);
	// Setplayer
	dynamic_pointer_cast<HeadRollerDieState>(death)->SetPlayer(player);
	// Movement
	InitPos();

	// Property
	m_fRotationSpeed = 3.0f;
}

void HeadRollerMovement::Tick()
{
	CheckCoolDown();
	CheckHit();

	currentState->Tick();
	bool currentStateEnd = !currentState->IsPlaying();

	switch (currentStateId)
	{
	case ENEMY_EMPYT:
		break;
	case ENEMY_S_IDLE:
		CheckPlayer();
		break;
	case ENEMY_S_WALK: // 종료 없는 state
	{
		CheckPlayer();
		if (!currentStateEnd)
		{
			Walk();
		}
	}
	break;
	case ENEMY_S_ATTACK:
	{
		if (currentStateEnd)
		{
			// 두리번 state로 전환
			ChangeState(look);
			// attackCooltime 계산 시작
			countAttackCool = true;

		}
	}
	break;
	case ENEMY_S_LOOK:
	{
		if (currentStateEnd)
		{
			ChangeState(walk);
			InitPosAfterRoll();
		}
	}
	break;
	case ENEMY_S_HIT:
		if (currentStateEnd)
		{
			ChangeState(look);
			//InitPos();
		}
		break;
	case ENEMY_S_DEATH:
		if (currentStateEnd)
		{
			GetOwner()->m_bDelete = true;
		}
		break;
	}


}

shared_ptr<UScriptComponent> HeadRollerMovement::Clone()
{
	auto script = make_shared<HeadRollerMovement>();
	return script;
}

void HeadRollerMovement::InitPos()
{
	auto look = GetOwner()->GetLook();
	m_vPos[0] = GetOwner()->GetPosition() + posOffset * look;
	m_vPos[1] = GetOwner()->GetPosition() - posOffset * look;

	m_vCurrentTarget = m_vPos[0];
	m_currentPosIdx = 0;
	m_vDistance = m_vCurrentTarget - GetOwner()->GetPosition();

	//look = -look;
	m_targetYaw = atan2f(look.x, look.z);

	m_rotate = true;
}

void HeadRollerMovement::InitPosAfterRoll()
{
	// direction 을 사용해서 pos 찾기

	m_vPos[0] = GetOwner()->GetPosition() + 4.0f * -direction;
	m_vPos[1] = GetOwner()->GetPosition() - 1.0f * -direction;

	m_vCurrentTarget = m_vPos[0];
	m_currentPosIdx = 0;
	m_vDistance = m_vCurrentTarget - GetOwner()->GetPosition();

	//look = -look;
	m_targetYaw = atan2f(-direction.x, -direction.z);

	m_rotate = true;
}

void HeadRollerMovement::Walk()
{
	Vec3 pos = GetOwner()->GetPosition();
	Vec3 diff = m_vCurrentTarget - pos;
	if (diff.Length() < 0.1f)
	{
		// 다음 목표 설정
		m_currentPosIdx = (m_currentPosIdx + 1) % 2;
		m_vCurrentTarget = m_vPos[m_currentPosIdx];

		m_vDistance = m_vCurrentTarget - GetOwner()->GetPosition();

		// direction을 이용한 회전
		auto look = GetOwner()->GetLook();
		look = -look;
		m_targetYaw = atan2f(look.x, look.z);
		m_rotate = true;

	}
	else if (m_rotate)
	{
		if (LerpRotate(m_targetYaw))
		{
			m_rotate = false;
		}
	}
	else
	{
		// 이동
		Vec3 pos = m_vDistance * m_walkSpeed;
		GetOwner()->AddPosition(pos);
	}
}

bool HeadRollerMovement::CheckPlayer()
{
	Vec3 dis = player.lock()->GetPosition() - GetOwner()->GetPosition();

	if (dis.Length() < chasingOffset)
	{
		direction = dis;
		direction.y = 0;
		direction.Normalize();

		// Roll 
		AttackStart();
		return true;
	}
	else
	{
		ChangeState(walk);
		return false;
	}
}

void HeadRollerMovement::CheckCoolDown()
{
	if (countAttackCool)
	{
		currentACool += TIMER->GetDeltaTime();
		if (currentACool >= attackCool)
		{
			canAttack = true;
		}
	}
}

void HeadRollerMovement::CheckHit()
{
	// HIT
	auto enemy = dynamic_pointer_cast<TEnemy>(GetOwner());
	bool isHit = enemy->CheckHit();
	if (isHit && !enemy->IsDead())
	{
		//// 회전 
		//Vec3 direction = GetOwner()->GetPosition() - player.lock()->GetPosition();
		//direction.y = 0;
		//direction.Normalize();
		//Vec3 tempUp = { 0.0f, 1.0f, 0.0f };
		//Vec3 moveDir = tempUp.Cross(direction); // 반시계 방향
		//float targetYaw = atan2f(moveDir.x, moveDir.z);
		//Vec3 currentRot = GetOwner()->GetRotation();
		//currentRot.y = targetYaw;
		//GetOwner()->SetRotation(currentRot);
		//m_rotate = false;

		ChangeState(hit);
	}
	if (enemy->IsDead())
	{
		ChangeState(death);
	}
}

void HeadRollerMovement::AttackStart()
{
	canAttack = false;
	static_pointer_cast<HeadRollerAttackState>(roll)->SetDirection(direction);
	ChangeState(roll);

}
