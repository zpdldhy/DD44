#include "pch.h"
#include "WalkerStates.h"
#include "AActor.h"
#include "USkinnedMeshComponent.h"
#include "UAnimInstance.h"
#include "Sound.h"


WalkerIdleState::WalkerIdleState(weak_ptr<AActor> _pOwner) : StateBase(ENEMY_S_IDLE)
{
	m_pOwner = _pOwner;
	m_bCanInterrupt = true;
}

void WalkerIdleState::Enter()
{
	// �⺻ state ����
	m_bOnPlaying = true;
	// �ִϸ��̼� Idle �÷���
	auto animInstance = m_pOwner.lock()->GetMeshComponent<USkinnedMeshComponent>()->GetAnimInstance();
	int idleIndex = animInstance->GetAnimIndex(L"Armature|Idle_look");
	animInstance->SetCurrentAnimTrack(idleIndex);
}

void WalkerIdleState::Tick()
{
}

void WalkerIdleState::End()
{
	// �⺻ state ����
	m_bOnPlaying = false;

}

WalkerWalkState::WalkerWalkState(weak_ptr<AActor> _pOwner) : StateBase(ENEMY_S_WALK)
{
	m_pOwner = _pOwner;
	m_bCanInterrupt = true;
}

void WalkerWalkState::Enter()
{
	// �⺻ state ����
	m_bOnPlaying = true;
	// �ִϸ��̼� Idle �÷���
	auto animInstance = m_pOwner.lock()->GetMeshComponent<USkinnedMeshComponent>()->GetAnimInstance();
	int idleIndex = animInstance->GetAnimIndex(L"Armature|Walk");
	animInstance->SetCurrentAnimTrack(idleIndex);
}

void WalkerWalkState::Tick()
{
}

void WalkerWalkState::End()
{
	// �⺻ state ����
	m_bOnPlaying = false;
}

WalkerHitState::WalkerHitState(weak_ptr<AActor> _pOwner) : StateBase(ENEMY_S_HIT)
{
	m_pOwner = _pOwner;
	m_bCanInterrupt = false;
}

void WalkerHitState::Enter()
{
	// �⺻ state ����
	m_bOnPlaying = true;
	// �ִϸ��̼� Idle �÷���
	auto animInstance = m_pOwner.lock()->GetMeshComponent<USkinnedMeshComponent>()->GetAnimInstance();
	int index = animInstance->GetAnimIndex(L"Armature|Stun");
	animInstance->PlayOnce(index);
	// ����
	SOUNDMANAGER->GetPtr(ESoundType::Enemy_Damaged)->PlayEffect2D();
}

void WalkerHitState::Tick()
{
	auto animInstance = m_pOwner.lock()->GetMeshComponent<USkinnedMeshComponent>()->GetAnimInstance();
	if (!animInstance->m_bOnPlayOnce)
	{
		// �ִϸ��̼� ����
		End();
	}
}

void WalkerHitState::End()
{
	// �⺻ state ����
	m_bOnPlaying = false;

}

WalkerDieState::WalkerDieState(weak_ptr<AActor> _pOwner) : StateBase(ENEMY_S_DEATH)
{
	m_pOwner = _pOwner;
	m_bCanInterrupt = false;
}

void WalkerDieState::Enter()
{
	// �⺻ state ����
	m_bOnPlaying = true;
	// �ִϸ��̼� Idle �÷���
	auto animInstance = m_pOwner.lock()->GetMeshComponent<USkinnedMeshComponent>()->GetAnimInstance();
	int index = animInstance->GetAnimIndex(L"Armature|Stun");
	animInstance->SetKeyFrame(index, 30);
	animInstance->PlayOnce(index);

}

void WalkerDieState::Tick()
{
	auto animInstance = m_pOwner.lock()->GetMeshComponent<USkinnedMeshComponent>()->GetAnimInstance();
	if (!animInstance->m_bOnPlayOnce)
	{
		// ����
		animInstance->m_bPlay = false;
		End();
	}

}

void WalkerDieState::End()
{
	// �⺻ state ����
	m_bOnPlaying = false;

}
