#include "pch.h"
#include "BatStates.h"
#include "AActor.h"
#include "USkinnedMeshComponent.h"
#include "UAnimInstance.h"

BatIdleState::BatIdleState(weak_ptr<AActor> _pOwner) : StateBase(ENEMY_S_IDLE)
{
	m_pOwner = _pOwner;
	m_bCanInterrupt = true;
}

void BatIdleState::Enter()
{
	// �⺻ state ����
	m_bOnPlaying = true;
	// �ִϸ��̼� Idle �÷���
	auto animInstance = m_pOwner.lock()->GetMeshComponent<USkinnedMeshComponent>()->GetAnimInstance();
	int idleIndex = animInstance->GetAnimIndex(L"Idle");
	animInstance->SetCurrentAnimTrack(idleIndex);

}

void BatIdleState::Tick()
{
}

void BatIdleState::End()
{
	// �⺻ state ����
	m_bOnPlaying = false;
}

BatWalkState::BatWalkState(weak_ptr<AActor> _pOwner) : StateBase(ENEMY_S_WALK)
{
	m_pOwner = _pOwner;
	m_bCanInterrupt = true;
}

void BatWalkState::Enter()
{
	// �⺻ state ����
	m_bOnPlaying = true;
	// �ִϸ��̼� Idle �÷���
	auto animInstance = m_pOwner.lock()->GetMeshComponent<USkinnedMeshComponent>()->GetAnimInstance();
	int idleIndex = animInstance->GetAnimIndex(L"Fly");
	animInstance->SetCurrentAnimTrack(idleIndex);
}

void BatWalkState::Tick()
{
}

void BatWalkState::End()
{
	// �⺻ state ����
	m_bOnPlaying = false;
}

BatAttackState::BatAttackState(weak_ptr<AActor> _pOwner) : StateBase(ENEMY_S_ATTACK)
{
	m_pOwner = _pOwner;
	m_bCanInterrupt = false;
}

void BatAttackState::Enter()
{
	// �⺻ state ����
	m_bOnPlaying = true;
	// �ִϸ��̼� Idle �÷���
	auto animInstance = m_pOwner.lock()->GetMeshComponent<USkinnedMeshComponent>()->GetAnimInstance();
	int index = animInstance->GetAnimIndex(L"Bite");
	animInstance->PlayOnce(index);
}

void BatAttackState::Tick()
{
	// �̶� ���� �̵��ϴ� �� ��� ó���ؾ����� �𸣰پ�
	auto animInstance = m_pOwner.lock()->GetMeshComponent<USkinnedMeshComponent>()->GetAnimInstance();
	if (!animInstance->m_bOnPlayOnce)
	{
		// �ִϸ��̼� ����
		End();
	}
}

void BatAttackState::End()
{
	// �⺻ state ����
	m_bOnPlaying = false;
}

BatDieState::BatDieState(weak_ptr<AActor> _pOwner) : StateBase(ENEMY_S_DEATH)
{
	m_pOwner = _pOwner;
	m_bCanInterrupt = false;
}

void BatDieState::Enter()
{
	// �⺻ state ����
	m_bOnPlaying = true;
	// �ִϸ��̼� Idle �÷���
	auto animInstance = m_pOwner.lock()->GetMeshComponent<USkinnedMeshComponent>()->GetAnimInstance();
	int index = animInstance->GetAnimIndex(L"Shock");
	animInstance->PlayOnce(index);
}

void BatDieState::Tick()
{
	auto animInstance = m_pOwner.lock()->GetMeshComponent<USkinnedMeshComponent>()->GetAnimInstance();
	if (!animInstance->m_bOnPlayOnce)
	{
		// �ִϸ��̼� ����
		End();
	}
	
}

void BatDieState::End()
{
	m_bOnPlaying = false;
}
