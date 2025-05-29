#include "pch.h"
#include "BettyStates.h"

#include "AActor.h"
#include "USkinnedMeshComponent.h"
#include "UAnimInstance.h"

BettyIdleState::BettyIdleState(weak_ptr<AActor> _pOwner) : StateBase(BETTY_S_IDLE)
{
	m_pOwner = _pOwner;
	m_bCanInterrupt = true;
}

void BettyIdleState::Enter()
{
	// �⺻ state ����
	m_bOnPlaying = true;
	// �ִϸ��̼� Idle �÷���
	auto animInstance = m_pOwner.lock()->GetMeshComponent<USkinnedMeshComponent>()->GetAnimInstance();
	int idleIndex = animInstance->GetAnimIndex(L"Armature|Idle_Beast");
	animInstance->SetCurrentAnimTrack(1);
}

void BettyIdleState::Tick()
{
	
}

void BettyIdleState::End()
{
	// �⺻ state ����
	m_bOnPlaying = false;
}

BettyIntroState::BettyIntroState(weak_ptr<AActor> _pOwner) : StateBase(BETTY_S_INTRO)
{
	m_pOwner = _pOwner;
	m_bCanInterrupt = false;
}

void BettyIntroState::Enter()
{
	// �⺻ state ����
	m_bOnPlaying = true;
	// �ִϸ��̼� Idle �÷���
	auto animInstance = m_pOwner.lock()->GetMeshComponent<USkinnedMeshComponent>()->GetAnimInstance();
	int index = animInstance->GetAnimIndex(L"Armature|Intro");
	animInstance->PlayOnce(index);
}

void BettyIntroState::Tick()
{
	auto animInstance = m_pOwner.lock()->GetMeshComponent<USkinnedMeshComponent>()->GetAnimInstance();
	if (!animInstance->m_bOnPlayOnce)
	{
		if (m_bNextAnim)
		{
			int index = animInstance->GetAnimIndex(L"Armature|FistSlam");
			animInstance->PlayOnce(index);
			m_bNextAnim = false;
		}
		else
		{
			End();
		}
	}
}

void BettyIntroState::End()
{
	// �⺻ state ����
	m_bOnPlaying = false;
}
