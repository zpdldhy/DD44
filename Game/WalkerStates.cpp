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
	// 기본 state 세팅
	m_bOnPlaying = true;
	// 애니메이션 Idle 플레이
	auto animInstance = m_pOwner.lock()->GetMeshComponent<USkinnedMeshComponent>()->GetAnimInstance();
	int idleIndex = animInstance->GetAnimIndex(L"Armature|Idle_look");
	animInstance->SetCurrentAnimTrack(idleIndex);
}

void WalkerIdleState::Tick()
{
}

void WalkerIdleState::End()
{
	// 기본 state 세팅
	m_bOnPlaying = false;

}

WalkerWalkState::WalkerWalkState(weak_ptr<AActor> _pOwner) : StateBase(ENEMY_S_WALK)
{
	m_pOwner = _pOwner;
	m_bCanInterrupt = true;
}

void WalkerWalkState::Enter()
{
	// 기본 state 세팅
	m_bOnPlaying = true;
	// 애니메이션 Idle 플레이
	auto animInstance = m_pOwner.lock()->GetMeshComponent<USkinnedMeshComponent>()->GetAnimInstance();
	int idleIndex = animInstance->GetAnimIndex(L"Armature|Walk");
	animInstance->SetCurrentAnimTrack(idleIndex);
}

void WalkerWalkState::Tick()
{
}

void WalkerWalkState::End()
{
	// 기본 state 세팅
	m_bOnPlaying = false;
}

WalkerHitState::WalkerHitState(weak_ptr<AActor> _pOwner) : StateBase(ENEMY_S_HIT)
{
	m_pOwner = _pOwner;
	m_bCanInterrupt = false;
}

void WalkerHitState::Enter()
{
	// 기본 state 세팅
	m_bOnPlaying = true;
	// 애니메이션 Idle 플레이
	auto animInstance = m_pOwner.lock()->GetMeshComponent<USkinnedMeshComponent>()->GetAnimInstance();
	int index = animInstance->GetAnimIndex(L"Armature|Stun");
	animInstance->PlayOnce(index);
	// 사운드
	SOUNDMANAGER->GetPtr(ESoundType::Enemy_Damaged)->PlayEffect2D();
}

void WalkerHitState::Tick()
{
	auto animInstance = m_pOwner.lock()->GetMeshComponent<USkinnedMeshComponent>()->GetAnimInstance();
	if (!animInstance->m_bOnPlayOnce)
	{
		// 애니메이션 종료
		End();
	}
}

void WalkerHitState::End()
{
	// 기본 state 세팅
	m_bOnPlaying = false;

}

WalkerDieState::WalkerDieState(weak_ptr<AActor> _pOwner) : StateBase(ENEMY_S_DEATH)
{
	m_pOwner = _pOwner;
	m_bCanInterrupt = false;
}

void WalkerDieState::Enter()
{
	// 기본 state 세팅
	m_bOnPlaying = true;
	// 애니메이션 Idle 플레이
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
		// 종료
		animInstance->m_bPlay = false;
		End();
	}

}

void WalkerDieState::End()
{
	// 기본 state 세팅
	m_bOnPlaying = false;

}
