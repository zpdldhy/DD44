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
	// 기본 state 세팅
	m_bOnPlaying = true;
	// 애니메이션 Idle 플레이
	auto animInstance = m_pOwner.lock()->GetMeshComponent<USkinnedMeshComponent>()->GetAnimInstance();
	int idleIndex = animInstance->GetAnimIndex(L"Idle_0");
	animInstance->SetCurrentAnimTrack(idleIndex);


}

void PlayerIdleState::Tick()
{

}

void PlayerIdleState::End()
{
	// 기본 state 세팅
	m_bOnPlaying = false;
}

PlayerWalkState::PlayerWalkState(weak_ptr<AActor> _pOwner) : StateBase(PLAYER_S_WALK)
{
	m_pOwner = _pOwner;
	m_bCanInterrupt = true;
}

void PlayerWalkState::Enter()
{
	// 기본 state 세팅
	m_bOnPlaying = true;

	// 애니메이션 Walk 플레이
	auto animInstance = m_pOwner.lock()->GetMeshComponent<USkinnedMeshComponent>()->GetAnimInstance();
	int idleIndex = animInstance->GetAnimIndex(L"Run");
	animInstance->SetCurrentAnimTrack(idleIndex);
}

void PlayerWalkState::Tick()
{
}

void PlayerWalkState::End()
{
	// 기본 state 세팅
	m_bOnPlaying = false;
}

PlayerAttackState::PlayerAttackState(weak_ptr<AActor> _pOwner) : StateBase(PLAYER_S_ATTACK)
{
	m_pOwner = _pOwner;
	m_bCanInterrupt = false;
}

void PlayerAttackState::Enter()
{
	// 기본 state 세팅
	m_bOnPlaying = true;

	// 애니메이션 Attack 플레이
	auto animInstance = m_pOwner.lock()->GetMeshComponent<USkinnedMeshComponent>()->GetAnimInstance();
	int attackIndex = animInstance->GetAnimIndex(L"Slash_Light_R_new");
	animInstance->PlayOnce(attackIndex);

	// Sword 위치 설정
	int index = m_pBackSocket.lock()->GetChildIndex(m_pSword.lock());
	if (index < 0)
	{
		assert(false);
	}
	m_pBackSocket.lock()->RemoveChild(index);
	m_pHandSocket.lock()->AddChild(m_pSword.lock());

	m_pSword.lock()->GetMeshTransform()->SetLocalPosition(handSwordPos);
	m_pSword.lock()->GetMeshTransform()->SetLocalRotation(handSwordRot);

}

void PlayerAttackState::Tick()
{
	auto animInstance = m_pOwner.lock()->GetMeshComponent<USkinnedMeshComponent>()->GetAnimInstance();
	if (!animInstance->m_bOnPlayOnce)
	{
		// 애니메이션 종료
		*m_pCurrentState = m_pPrevState;
		m_pPrevState->Enter();
		End();
	}
}

void PlayerAttackState::End()
{
	// 기본 state 세팅
	m_bOnPlaying = false;

	// Sword 위치 설정
	int index = m_pHandSocket.lock()->GetChildIndex(m_pSword.lock());
	if (index < 0)
	{
		assert(false);
	}
	m_pHandSocket.lock()->RemoveChild(index);
	m_pBackSocket.lock()->AddChild(m_pSword.lock());

	m_pSword.lock()->GetMeshTransform()->SetLocalPosition(backSwordPos);
	m_pSword.lock()->GetMeshTransform()->SetLocalRotation(backSwordRot);

	// 참조 관리
	m_pCurrentState = nullptr;
	m_pPrevState.reset();
}

void PlayerAttackState::SetComponent(weak_ptr<UMeshComponent> _sword, weak_ptr<UMeshComponent> _hand, weak_ptr<UMeshComponent> _back)
{
	m_pSword = _sword;
	m_pHandSocket = _hand;
	m_pBackSocket = _back;
}

