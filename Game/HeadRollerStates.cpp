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
	// �⺻ state ����
	m_bOnPlaying = true;
	// �ִϸ��̼� Idle �÷���
	auto animInstance = m_pOwner.lock()->GetMeshComponent<USkinnedMeshComponent>()->GetAnimInstance();
	int idleIndex = animInstance->GetAnimIndex(L"Armature|Idle_look");
	animInstance->SetCurrentAnimTrack(idleIndex);
}

void Enemy1IdleState::Tick()
{
}

void Enemy1IdleState::End()
{
	// �⺻ state ����
	m_bOnPlaying = false;
}

Enemy1WalkState::Enemy1WalkState(weak_ptr<AActor> _pOwner) : StateBase(ENEMY_S_WALK)
{
	m_pOwner = _pOwner;
	m_bCanInterrupt = true;
}

void Enemy1WalkState::Enter()
{
	// �⺻ state ����
	m_bOnPlaying = true;
	// �ִϸ��̼� Walk �÷���
	auto animInstance = m_pOwner.lock()->GetMeshComponent<USkinnedMeshComponent>()->GetAnimInstance();
	int idleIndex = animInstance->GetAnimIndex(L"Armature|Walk");
	animInstance->SetCurrentAnimTrack(idleIndex);
}

void Enemy1WalkState::Tick()
{

}

void Enemy1WalkState::End()
{
	// �⺻ state ����
	m_bOnPlaying = false;
}

Enemy1AttackStartState::Enemy1AttackStartState(weak_ptr<AActor> _pOwner) : StateBase(ENEMY_S_ATTACK)
{
	m_pOwner = _pOwner;
	m_bCanInterrupt = false;
}

void Enemy1AttackStartState::Enter()
{
	// �⺻ state ����
	m_bOnPlaying = true;
	// �ִϸ��̼� Walk �÷���
	auto animInstance = m_pOwner.lock()->GetMeshComponent<USkinnedMeshComponent>()->GetAnimInstance();
	int idleIndex = animInstance->GetAnimIndex(L"Armature|roll_start");
	animInstance->SetCurrentAnimTrack(idleIndex);
}

void Enemy1AttackStartState::Tick()
{
	auto animInstance = m_pOwner.lock()->GetMeshComponent<USkinnedMeshComponent>()->GetAnimInstance();
	if (!animInstance->m_bOnPlayOnce)
	{
		// Roll_loop ����
		*m_pCurrentState = m_pRollState;
		m_pRollState->Enter();
		End();
	}
}

void Enemy1AttackStartState::End()
{
	// �⺻ state ����
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
	// �⺻ state ����
	m_bOnPlaying = true;
	// �ִϸ��̼� Walk �÷���
	auto animInstance = m_pOwner.lock()->GetMeshComponent<USkinnedMeshComponent>()->GetAnimInstance();
	int idleIndex = animInstance->GetAnimIndex(L"Armature|roll_loop");
	animInstance->SetCurrentAnimTrack(idleIndex);
}

void Enemy1RollState::Tick()
{
	auto animInstance = m_pOwner.lock()->GetMeshComponent<USkinnedMeshComponent>()->GetAnimInstance();
	if (!animInstance->m_bOnPlayOnce)
	{
		// Roll_loop ����
		*m_pCurrentState = m_pRollState;
		m_pRollState->Enter();
		End();
	}
}

void Enemy1RollState::End()
{
	// �⺻ state ����
	m_bOnPlaying = false;
}

Enemy1StunState::Enemy1StunState(weak_ptr<AActor> _pOwner) : StateBase(ENEMY_S_HIT)
{
	m_pOwner = _pOwner;
	m_bCanInterrupt = false;
}

void Enemy1StunState::Enter()
{
	// �⺻ state ����
	m_bOnPlaying = true;
	// �ִϸ��̼� Walk �÷���
	auto animInstance = m_pOwner.lock()->GetMeshComponent<USkinnedMeshComponent>()->GetAnimInstance();
	int idleIndex = animInstance->GetAnimIndex(L"Armature|Stun");
	animInstance->SetCurrentAnimTrack(idleIndex);
}

void Enemy1StunState::Tick()
{
}

void Enemy1StunState::End()
{
	// �⺻ state ����
	m_bOnPlaying = false;
}
