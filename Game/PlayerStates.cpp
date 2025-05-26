#include "pch.h"
#include "PlayerStates.h"
#include "AActor.h"
#include "USkinnedMeshComponent.h"
#include "UAnimInstance.h"
#include "MeshTransform.h"

PlayerIdleState::PlayerIdleState(weak_ptr<AActor> _pOwner) : StateBase(PLAYER_S_IDLE)
{
	m_pOwner = _pOwner;
	m_bCanInterrupt = true;
}

void PlayerIdleState::Enter()
{
	// �⺻ state ����
	m_bOnPlaying = true;
	// �ִϸ��̼� Idle �÷���
	auto animInstance = m_pOwner.lock()->GetMeshComponent<USkinnedMeshComponent>()->GetAnimInstance();
	int idleIndex = animInstance->GetAnimIndex(L"Idle_0");
	animInstance->SetCurrentAnimTrack(idleIndex);


}

void PlayerIdleState::Tick()
{

}

void PlayerIdleState::End()
{
	// �⺻ state ����
	m_bOnPlaying = false;
}

PlayerWalkState::PlayerWalkState(weak_ptr<AActor> _pOwner) : StateBase(PLAYER_S_WALK)
{
	m_pOwner = _pOwner;
	m_bCanInterrupt = true;
}

void PlayerWalkState::Enter()
{
	// �⺻ state ����
	m_bOnPlaying = true;

	// �ִϸ��̼� Walk �÷���
	auto animInstance = m_pOwner.lock()->GetMeshComponent<USkinnedMeshComponent>()->GetAnimInstance();
	int idleIndex = animInstance->GetAnimIndex(L"Run");
	animInstance->SetCurrentAnimTrack(idleIndex);
}

void PlayerWalkState::Tick()
{
}

void PlayerWalkState::End()
{
	// �⺻ state ����
	m_bOnPlaying = false;
}

PlayerAttackState::PlayerAttackState(weak_ptr<AActor> _pOwner) : StateBase(PLAYER_S_ATTACK)
{
	m_pOwner = _pOwner;
	m_bCanInterrupt = false;
}

void PlayerAttackState::Enter()
{
	// �⺻ state ����
	m_bOnPlaying = true;

	// �ִϸ��̼� Attack �÷���
	auto animInstance = m_pOwner.lock()->GetMeshComponent<USkinnedMeshComponent>()->GetAnimInstance();
	int attackIndex = animInstance->GetAnimIndex(L"Slash_Light_R_new");
	animInstance->PlayOnce(attackIndex);

	// Sword ��ġ ����
	int index = m_pBackSocket->GetChildIndex(m_pSword);
	if (index < 0)
	{
		assert(false);
	}
	m_pBackSocket->RemoveChild(index);
	m_pHandSocket->AddChild(m_pSword);

	m_pSword->SetLocalPosition(handSwordPos);
	m_pSword->SetLocalRotation(handSwordRot);

}

void PlayerAttackState::Tick()
{
	auto animInstance = m_pOwner.lock()->GetMeshComponent<USkinnedMeshComponent>()->GetAnimInstance();
	if (!animInstance->m_bOnPlayOnce)
	{
		// �ִϸ��̼� ����
		*m_pCurrentState = m_pPrevState;
		m_pPrevState->Enter();
		End();
	}
}

void PlayerAttackState::End()
{
	// �⺻ state ����
	m_bOnPlaying = false;

	// Sword ��ġ ����
	int index = m_pHandSocket->GetChildIndex(m_pSword);
	if (index < 0)
	{
		assert(false);
	}
	m_pHandSocket->RemoveChild(index);
	m_pBackSocket->AddChild(m_pSword);

	m_pSword->SetLocalPosition(backSwordPos);
	m_pSword->SetLocalRotation(backSwordRot);

	// ���� ����
	m_pCurrentState = nullptr;
	m_pPrevState.reset();
}

void PlayerAttackState::SetComponent(shared_ptr<UMeshComponent> _sword, shared_ptr<UMeshComponent> _hand, shared_ptr<UMeshComponent> _back)
{
	m_pSword = _sword;
	m_pHandSocket = _hand;
	m_pBackSocket = _back;
}

