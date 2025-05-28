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
	// 기본 state 세팅
	m_bOnPlaying = true;
	// 애니메이션 Idle 플레이
	auto animInstance = m_pOwner.lock()->GetMeshComponent<USkinnedMeshComponent>()->GetAnimInstance();
	int idleIndex = animInstance->GetAnimIndex(L"Idle");
	animInstance->SetCurrentAnimTrack(idleIndex);

}

void BatIdleState::Tick()
{
}

void BatIdleState::End()
{
	// 기본 state 세팅
	m_bOnPlaying = false;
}

BatWalkState::BatWalkState(weak_ptr<AActor> _pOwner) : StateBase(ENEMY_S_WALK)
{
	m_pOwner = _pOwner;
	m_bCanInterrupt = true;
}

void BatWalkState::Enter()
{
	// 기본 state 세팅
	m_bOnPlaying = true;
	// 애니메이션 Idle 플레이
	auto animInstance = m_pOwner.lock()->GetMeshComponent<USkinnedMeshComponent>()->GetAnimInstance();
	int idleIndex = animInstance->GetAnimIndex(L"Fly");
	animInstance->SetCurrentAnimTrack(idleIndex);
}

void BatWalkState::Tick()
{
}

void BatWalkState::End()
{
	// 기본 state 세팅
	m_bOnPlaying = false;
}

BatAttackState::BatAttackState(weak_ptr<AActor> _pOwner) : StateBase(ENEMY_S_ATTACK)
{
	m_pOwner = _pOwner;
	m_bCanInterrupt = false;
}

void BatAttackState::Enter()
{
	// 기본 state 세팅
	m_bOnPlaying = true;
	// 애니메이션 Idle 플레이
	auto animInstance = m_pOwner.lock()->GetMeshComponent<USkinnedMeshComponent>()->GetAnimInstance();
	int index = animInstance->GetAnimIndex(L"Bite");
	animInstance->PlayOnce(index);
}

void BatAttackState::Tick()
{
	// 이때 맞춰 이동하는 걸 어디서 처리해야할지 모르겟어
	auto animInstance = m_pOwner.lock()->GetMeshComponent<USkinnedMeshComponent>()->GetAnimInstance();
	if (!animInstance->m_bOnPlayOnce)
	{
		// 애니메이션 종료
		End();
	}
}

void BatAttackState::End()
{
	// 기본 state 세팅
	m_bOnPlaying = false;
}

BatDieState::BatDieState(weak_ptr<AActor> _pOwner) : StateBase(ENEMY_S_DEATH)
{
	m_pOwner = _pOwner;
	m_bCanInterrupt = false;
}

void BatDieState::Enter()
{
	// 기본 state 세팅
	m_bOnPlaying = true;
	// 애니메이션 Idle 플레이
	auto animInstance = m_pOwner.lock()->GetMeshComponent<USkinnedMeshComponent>()->GetAnimInstance();
	int index = animInstance->GetAnimIndex(L"Shock");
	animInstance->PlayOnce(index);
}

void BatDieState::Tick()
{
	auto animInstance = m_pOwner.lock()->GetMeshComponent<USkinnedMeshComponent>()->GetAnimInstance();
	if (!animInstance->m_bOnPlayOnce)
	{
		// 애니메이션 종료
		End();
	}
	
}

void BatDieState::End()
{
	m_bOnPlaying = false;
}
