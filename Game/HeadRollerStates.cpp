#include "pch.h"
#include "HeadRollerStates.h"
#include "AActor.h"
#include "USkinnedMeshComponent.h"
#include "UAnimInstance.h"

Enemy1IdleState::Enemy1IdleState(weak_ptr<AActor> _pOwner) : StateBase(ENEMY_S_IDLE)
{
	m_pOwner = _pOwner;
	m_bCanInterrupt = true;
}

void Enemy1IdleState::Enter()
{	
	// 기본 state 세팅
	m_bOnPlaying = true;
	// 애니메이션 Idle 플레이
	auto animInstance = m_pOwner.lock()->GetMeshComponent<USkinnedMeshComponent>()->GetAnimInstance();
	int idleIndex = animInstance->GetAnimIndex(L"Armature|Idle_look");
	animInstance->SetCurrentAnimTrack(idleIndex);
}

void Enemy1IdleState::Tick()
{
}

void Enemy1IdleState::End()
{
	// 기본 state 세팅
	m_bOnPlaying = false;
}

Enemy1WalkState::Enemy1WalkState(weak_ptr<AActor> _pOwner) : StateBase(ENEMY_S_WALK)
{
	m_pOwner = _pOwner;
	m_bCanInterrupt = true;
}

void Enemy1WalkState::Enter()
{
	// 기본 state 세팅
	m_bOnPlaying = true;
	// 애니메이션 Walk 플레이
	auto animInstance = m_pOwner.lock()->GetMeshComponent<USkinnedMeshComponent>()->GetAnimInstance();
	int idleIndex = animInstance->GetAnimIndex(L"Armature|Walk");
	animInstance->SetCurrentAnimTrack(idleIndex);
}

void Enemy1WalkState::Tick()
{

}

void Enemy1WalkState::End()
{
	// 기본 state 세팅
	m_bOnPlaying = false;
}

Enemy1AttackStartState::Enemy1AttackStartState(weak_ptr<AActor> _pOwner) : StateBase(ENEMY_S_ATTACK)
{
	m_pOwner = _pOwner;
	m_bCanInterrupt = false;
}

void Enemy1AttackStartState::Enter()
{
	// 기본 state 세팅
	m_bOnPlaying = true;
	// 애니메이션 Walk 플레이
	auto animInstance = m_pOwner.lock()->GetMeshComponent<USkinnedMeshComponent>()->GetAnimInstance();
	int idleIndex = animInstance->GetAnimIndex(L"Armature|roll_start");
	animInstance->SetCurrentAnimTrack(idleIndex);
}

void Enemy1AttackStartState::Tick()
{
	auto animInstance = m_pOwner.lock()->GetMeshComponent<USkinnedMeshComponent>()->GetAnimInstance();
	if (!animInstance->m_bOnPlayOnce)
	{
		// Roll_loop 시작
		*m_pCurrentState = m_pRollState;
		m_pRollState->Enter();
		End();
	}
}

void Enemy1AttackStartState::End()
{
	// 기본 state 세팅
	m_bOnPlaying = false;
}

void Enemy1AttackStartState::SetIdleState(const shared_ptr<StateBase>& _idle)
{
	m_pIdleState = _idle;
}
void Enemy1AttackStartState::SetRelateState(const shared_ptr<StateBase>& _roll, const shared_ptr<StateBase>& _stun)
{
	m_pRollState = _roll;
	m_pStunState = _stun;
}

Enemy1RollState::Enemy1RollState(weak_ptr<AActor> _pOwner) : StateBase(ENEMY_S_ATTACK)
{
	m_pOwner = _pOwner;
	m_bCanInterrupt = false;
}

void Enemy1RollState::Enter()
{
	// 기본 state 세팅
	m_bOnPlaying = true;
	// 애니메이션 Walk 플레이
	auto animInstance = m_pOwner.lock()->GetMeshComponent<USkinnedMeshComponent>()->GetAnimInstance();
	int idleIndex = animInstance->GetAnimIndex(L"Armature|roll_loop");
	animInstance->SetCurrentAnimTrack(idleIndex);
}

void Enemy1RollState::Tick()
{
	auto animInstance = m_pOwner.lock()->GetMeshComponent<USkinnedMeshComponent>()->GetAnimInstance();
	if (!animInstance->m_bOnPlayOnce)
	{
		// Roll_loop 시작
		*m_pCurrentState = m_pRollState;
		m_pRollState->Enter();
		End();
	}
}

void Enemy1RollState::End()
{
	// 기본 state 세팅
	m_bOnPlaying = false;
}

Enemy1StunState::Enemy1StunState(weak_ptr<AActor> _pOwner) : StateBase(ENEMY_S_HIT)
{
	m_pOwner = _pOwner;
	m_bCanInterrupt = false;
}

void Enemy1StunState::Enter()
{
	// 기본 state 세팅
	m_bOnPlaying = true;
	// 애니메이션 Walk 플레이
	auto animInstance = m_pOwner.lock()->GetMeshComponent<USkinnedMeshComponent>()->GetAnimInstance();
	int idleIndex = animInstance->GetAnimIndex(L"Armature|Stun");
	animInstance->SetCurrentAnimTrack(idleIndex);
}

void Enemy1StunState::Tick()
{
}

void Enemy1StunState::End()
{
	// 기본 state 세팅
	m_bOnPlaying = false;
}
