#include "pch.h"
#include "MageMovement.h"
#include "TEnemy.h"

#include "Timer.h"
#include "ObjectManager.h"
#include "TPlayer.h"
// temp temp temp !!!!!
#include "Input.h"
#include "EffectManager.h"

void MageMovement::Init()
{
	SetPlayer(dynamic_pointer_cast<TEnemy>(GetOwner())->GetPlayer());
	dynamic_pointer_cast<TEnemy>(GetOwner())->SetBloodPos(Vec2(2.0f, 3.0f));

	idle = make_shared<MageIdleState>(m_pOwner);
	appear = make_shared<MageAppearState>(m_pOwner);
	disappear = make_shared<MageDisappearState>(m_pOwner);
	hit = make_shared<MageHitState>(m_pOwner);
	attack = make_shared<MageAttackState>(m_pOwner);
	death = make_shared<MageDieState>(m_pOwner);
	ChangeState(idle);
	
	// HP
	dynamic_pointer_cast<TCharacter>(GetOwner())->SetHp(6);

	// staticMage
	if (GetOwner()->m_eActorType == ActorType::AT_STATICMONSTER)
	{
		bStaticMage = true;
		findDistance = 50.0f;
		dynamic_pointer_cast<MageHitState>(hit)->CheckStatic(bStaticMage);
		dynamic_pointer_cast<MageAttackState>(attack)->CheckStatic(bStaticMage);
		//dynamic_pointer_cast<MageDisappearState>(disappear)->CheckStatic(false);
	}

	// Player Pos
	dynamic_pointer_cast<MageDieState>(death)->SetPlayer(player);

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
	//	// 공격 받은 방향을 알 수 있을까 ? 
	//	ChangeState(death);
	//}

#pragma endregion
	// State
	currentState->Tick();
	if (currentStateId == ENEMY_S_DEATH)
	{
		if (!currentState->IsPlaying())
		{
			GetOwner()->m_bDelete = true;
		}
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
		// 공격 가능 확인 
		if (currentStateId != ENEMY_S_ATTACK && currentStateId != ENEMY_S_DEATH)
		{
			dynamic_pointer_cast<MageAttackState>(attack)->SetTarget(player);
			ChangeState(attack);
		}
	}
	else if(bStaticMage)
	{
		ChangeState(idle);
	}

	//
	Rotate();

	CheckHit();
}

shared_ptr<UScriptComponent> MageMovement::Clone()
{
	auto script = make_shared<MageMovement>();
	return script;
}

void MageMovement::ChangeState(shared_ptr<StateBase> _state)
{
	// 아래랑 중복되는 내용 아니야 ? ( 확인 필요 ) 
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
	// HP
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