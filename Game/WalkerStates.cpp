#include "pch.h"
#include "WalkerStates.h"
#include "AActor.h"
#include "USkinnedMeshComponent.h"
#include "UAnimInstance.h"
#include "Sound.h"
#include "Timer.h"
#include "EffectManager.h"
#include "TPlayer.h"

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
	int index = animInstance->GetAnimIndex(L"Armature|StartleHop");
	animInstance->PlayOnce(index);
	// ����
	SOUND->GetPtr(ESoundType::Enemy_Damaged)->PlayEffect2D();
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
	animInstance->m_fAnimPlayRate = 20.0f;
	animInstance->PlayOnce(index);
	
	Vec3 playerPos = m_pPlayer.lock()->GetPosition();
	playerPos.y += 1.5f;
	Vec3 monPos = m_pOwner.lock()->GetPosition();
	monPos.y += 1.5f;
	Vec3 soulDirection = playerPos - m_pOwner.lock()->GetPosition();
	soulDirection.Normalize();
	EFFECT->PlayEffect(EEffectType::Soul, monPos, 0, soulDirection, 1.0f, playerPos);

	// ����
	SOUND->GetPtr(ESoundType::Dead_Walker)->PlayEffect2D();

	// ��ġ
	Vec3 currentPos = m_pOwner.lock()->GetPosition();
	currentPos.y -= 1.0f;
	m_pOwner.lock()->SetPosition(currentPos);
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

	float frameTime = static_cast<float>(animInstance->GetTotalFrame());
	frameTime /= 30;
	m_fDissolveTimer += TIMER->GetDeltaTime();
	float t = m_fDissolveTimer / frameTime;
	auto comp = m_pOwner.lock()->GetMeshComponent<USkinnedMeshComponent>();
	ApplyDissolveToAllMaterials(comp, t);
}


void WalkerDieState::ApplyDissolveToAllMaterials(shared_ptr<class UMeshComponent> _comp, float _time)
{
	if (!_comp) return;

	shared_ptr<UMaterial> mat = _comp->GetMaterial();
	if (mat)
	{
		mat->SetDissolve(_time);
	}

	for (int i = 0; i < _comp->GetChildCount(); ++i)
	{
		ApplyDissolveToAllMaterials(_comp->GetChild(i), _time);
	}
}


void WalkerDieState::End()
{
	// �⺻ state ����
	m_bOnPlaying = false;

}
