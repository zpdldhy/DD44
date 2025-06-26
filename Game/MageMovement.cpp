#include "pch.h"
#include "MageMovement.h"
#include "TEnemy.h"

#include "Timer.h"
#include "ObjectManager.h"
#include "TPlayer.h"
#include "EffectManager.h"

void MageMovement::Init()
{
	auto player = dynamic_pointer_cast<TPlayer>(dynamic_pointer_cast<TEnemy>(GetOwner())->GetPlayer().lock());
	SetPlayer(player);

	dynamic_pointer_cast<TEnemy>(GetOwner())->SetBloodPos(Vec2(2.0f, 3.0f));

	idle = make_shared<MageIdleState>(m_pOwner);
	appear = make_shared<MageAppearState>(m_pOwner);
	disappear = make_shared<MageDisappearState>(m_pOwner);
	runaway = make_shared<MageRunaway>(m_pOwner);
	hit = make_shared<MageHitState>(m_pOwner);
	attack = make_shared<MageAttackState>(m_pOwner);
	death = make_shared<MageDieState>(m_pOwner);
	ChangeState(idle);
	
	// HP
	dynamic_pointer_cast<TCharacter>(GetOwner())->SetHp(6);
	dynamic_pointer_cast<TCharacter>(GetOwner())->SetSoul(5);


	// staticMage
	if (GetOwner()->m_eActorType == ActorType::AT_STATICMONSTER)
	{
		bStaticMage = true;
		findDistance = 25.0f;
		dynamic_pointer_cast<MageHitState>(hit)->CheckStatic(bStaticMage);
		dynamic_pointer_cast<MageAttackState>(attack)->CheckStatic(bStaticMage);
	}

	// Player Pos
	dynamic_pointer_cast<MageDieState>(death)->SetPlayer(player);

}

void MageMovement::Tick()
{
	currentState->Tick();
	bool currentEnd = !currentState->IsPlaying();

	// State
	switch (currentStateId)
	{
	case ENEMY_S_IDLE:
		CheckAttack();
		break;
	case ENEMY_S_ATTACK:
		if (currentEnd)
		{
			ChangeState(idle);
		}
		break;
	case ENEMY_S_HIT:
		if (currentEnd)
		{
			ChangeState(runaway);
		}
		break;
	case ENEMY_S_WALK:
		if (currentEnd)
		{
			ChangeState(disappear);
		}
		break;
	case ENEMY_S_APPEAR:
		if (currentEnd)
		{
			ChangeState(idle);
		}
		break;
	case ENEMY_S_DISAPPEAR:
		if (currentEnd)
		{
			ChangeState(appear);
		}
		break;
	case ENEMY_S_DEATH:
		if (currentEnd)
		{
			GetOwner()->m_bDelete = true;
			return;
		}
		break;
	default:
		break;
	}

	//
	CheckHit();
	Rotate();
}

shared_ptr<UScriptComponent> MageMovement::Clone()
{
	auto script = make_shared<MageMovement>();
	return script;
}

void MageMovement::Rotate()
{
	if (currentStateId == ENEMY_S_DEATH)
	{
		return;
	}
	Vec3 dir = player.lock()->GetPosition() - m_pOwner.lock()->GetPosition();
	float targetYaw = atan2f(dir.x, dir.z);

	LerpRotate(targetYaw);
}

void MageMovement::CheckHit()
{
	auto comp = dynamic_pointer_cast<TEnemy>(GetOwner());
	bool isHit = comp->CheckHit();
	if (isHit && !comp->IsDead())
	{
		//ChangeState(hit);
	}
	if (comp->IsDead())
	{
		ChangeState(death);
		
	}
}

void MageMovement::CheckAttack()
{
	// Attack
	auto distance = player.lock()->GetPosition() - GetOwner()->GetPosition();
	if (distance.Length() < findDistance)
	{
		// 공격 가능 확인 
		if (currentStateId != ENEMY_S_ATTACK && currentStateId != ENEMY_S_DEATH)
		{
			dynamic_pointer_cast<MageAttackState>(attack)->SetTarget(player);
			ChangeState(attack);
		}
	}
	else if (bStaticMage)
	{
		ChangeState(idle);
	}
}
