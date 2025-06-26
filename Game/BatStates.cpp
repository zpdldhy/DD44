#include "pch.h"
#include "BatStates.h"
#include "AActor.h"
#include "USkinnedMeshComponent.h"
#include "UMeshComponent.h"
#include "UAnimInstance.h"
#include "Sound.h"
#include "Timer.h"
#include "EffectManager.h"

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

	animInstance->AddEvent(index, attackStartFrame, [this]() {
		this->EnableAttackRange();
		});

	animInstance->AddEvent(index, attackEndFrame, [this]() {
		this->DisableAttackRange();
		});

	animInstance->PlayOnce(index);
	// ����
	SOUND->GetPtr(ESoundType::Attack_Bat)->PlayEffect2D();
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
	bActiveRange = false;
}

void BatAttackState::EnableAttackRange()
{
	bActiveRange = true;

	m_pAttackRange->m_bCollision = bActiveRange;
}

void BatAttackState::DisableAttackRange()
{
	bActiveRange = false;

	m_pAttackRange->m_bCollision = bActiveRange;
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

	Vec3 playerPos = m_pPlayer.lock()->GetPosition();
	playerPos.y += 1.5f;
	Vec3 monPos = m_pOwner.lock()->GetPosition();
	monPos.y += 1.5f;
	Vec3 soulDirection = playerPos - m_pOwner.lock()->GetPosition();
	soulDirection.Normalize();
	EFFECT->PlayEffect(EEffectType::Soul, monPos, 0, soulDirection, 1.0f, playerPos);


	// ����
	SOUND->GetPtr(ESoundType::Enemy_Damaged)->PlayEffect2D();
}

void BatDieState::Tick()
{
	auto animInstance = m_pOwner.lock()->GetMeshComponent<USkinnedMeshComponent>()->GetAnimInstance();
	if (!animInstance->m_bOnPlayOnce)
	{
		// �ִϸ��̼� ����
		End();
	}

	//Dissolve
	float frameTime = static_cast<float>(animInstance->GetTotalFrame());
	frameTime /= 30;
	m_fDissolveTimer += TIMER->GetDeltaTime();
	float t = m_fDissolveTimer / frameTime;
	auto comp = m_pOwner.lock()->GetMeshComponent<USkinnedMeshComponent>();
	ApplyDissolveToAllMaterials(comp, t);
}


void BatDieState::ApplyDissolveToAllMaterials(shared_ptr<UMeshComponent> _comp, float _time)
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

void BatDieState::End()
{
	m_bOnPlaying = false;
}
