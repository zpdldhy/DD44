#include "pch.h"
#include "MageStates.h"
#include "USkinnedMeshComponent.h"
#include "UAnimInstance.h"
#include "AActor.h"

#include "Timer.h"
#include "ProjectileManager.h"

// TEMP
#include "Input.h"

Vec3 V_Clamp(const Vec3& v, const Vec3& minV, const Vec3& maxV)
{
	return Vec3(
		max(minV.x, std::min(maxV.x, v.x)),
		max(minV.y, std::min(maxV.y, v.y)),
		max(minV.z, std::min(maxV.z, v.z))
	);
}

MageIdleState::MageIdleState(weak_ptr<AActor> _pOwner) : StateBase(ENEMY_S_IDLE)
{
	m_pOwner = _pOwner;
	m_bCanInterrupt = true;
}
void MageIdleState::Enter()
{
	// 기본 state 세팅
	m_bOnPlaying = true;
	// 애니메이션 Idle 플레이
	auto animInstance = m_pOwner.lock()->GetMeshComponent<USkinnedMeshComponent>()->GetAnimInstance();
	int idleIndex = animInstance->GetAnimIndex(L"Idle2");
	animInstance->SetCurrentAnimTrack(idleIndex);
}
void MageIdleState::Tick()
{
	if (INPUT->GetButton(C))
	{
		auto animInstance = m_pOwner.lock()->GetMeshComponent<USkinnedMeshComponent>()->GetAnimInstance();
		if (++animIndex >= animInstance->GetAnimTrackList().size())
		{
			animIndex = 0;
		}
		animInstance->SetCurrentAnimTrack(animIndex);
	}
}
void MageIdleState::End()
{
	// 기본 state 세팅
	m_bOnPlaying = false;
}

MageAppearState::MageAppearState(weak_ptr<AActor> _pOwner) : StateBase(ENEMY_S_IDLE)
{
	m_pOwner = _pOwner;
	m_bCanInterrupt = false;
}
void MageAppearState::Enter()
{
	// 기본 state 세팅
	m_bOnPlaying = true;
	// 애니메이션 Idle 플레이
	auto animInstance = m_pOwner.lock()->GetMeshComponent<USkinnedMeshComponent>()->GetAnimInstance();
	int idleIndex = animInstance->GetAnimIndex(L"Teleport_in");
	animInstance->PlayOnce(idleIndex);
	//m_pOwner.lock()->SetScale(Vec3(0.01f, 0.01f, 0.01f));

	//targetYaw = targetYaw = atan2f(dir.x, dir.z);
	//Vec3 currentRot = m_pOwner.lock()->GetRotation();
	//currentRot.y = targetYaw;
	//m_pOwner.lock()->SetRotation(currentRot);

	m_pOwner.lock()->m_bCollision = true;
	m_pOwner.lock()->m_bRender = true;
	m_pOwner.lock()->GetShapeComponent()->m_bVisible = true;
}
void MageAppearState::Tick()
{
	auto animInstance = m_pOwner.lock()->GetMeshComponent<USkinnedMeshComponent>()->GetAnimInstance();
	if (!animInstance->m_bOnPlayOnce)
	{
		// 애니메이션 종료
		m_pOwner.lock()->SetScale(Vec3(1.0f, 1.0f, 1.0f));
		End();
	}

	// Scale
	Vec3 scale = m_pOwner.lock()->GetScale();
	scale += increment;
	scale = V_Clamp(scale, minV, maxV);

	m_pOwner.lock()->SetScale(scale);

}
void MageAppearState::End()
{
	// 기본 state 세팅
	m_bOnPlaying = false;
}
void MageAppearState::SetDirection(Vec3 _targetPos)
{
	dir = _targetPos - m_pOwner.lock()->GetPosition();
}

MageDisappearState::MageDisappearState(weak_ptr<AActor> _pOwner) : StateBase(ENEMY_S_IDLE)
{
	m_pOwner = _pOwner;
	m_bCanInterrupt = false;
}
void MageDisappearState::Enter()
{
	// 기본 state 세팅
	m_bOnPlaying = true;
	// 애니메이션 Idle 플레이
	auto animInstance = m_pOwner.lock()->GetMeshComponent<USkinnedMeshComponent>()->GetAnimInstance();
	int idleIndex = animInstance->GetAnimIndex(L"Teleport");
	animInstance->PlayOnce(idleIndex);
}
void MageDisappearState::Tick()
{
	// Scale
	Vec3 scale = m_pOwner.lock()->GetScale();
	scale -= increment;
	scale = V_Clamp(scale, minV, scale);
	// scale이 일정 이하면 없어지게 처리 필요할 듯 
	m_pOwner.lock()->SetScale(scale);
	lowerElapsed += TIMER->GetDeltaTime();
	if (lowerElapsed > 1.6f)
	{
		m_pOwner.lock()->m_bRender = false;
		lowerElapsed = 0.0f;
	}
	auto animInstance = m_pOwner.lock()->GetMeshComponent<USkinnedMeshComponent>()->GetAnimInstance();
	if (!animInstance->m_bOnPlayOnce)
	{
		// 애니메이션 종료
		End();
	}
}
void MageDisappearState::End()
{
	// 기본 state 세팅
	m_bOnPlaying = false;
}

MageHitState::MageHitState(weak_ptr<AActor> _pOwner) : StateBase(ENEMY_S_HIT)
{
	m_pOwner = _pOwner;
	m_bCanInterrupt = false;
}
void MageHitState::Enter()
{
	// 기본 state 세팅
	m_bOnPlaying = true;
	// 애니메이션 Idle 플레이
	auto animInstance = m_pOwner.lock()->GetMeshComponent<USkinnedMeshComponent>()->GetAnimInstance();
	int idleIndex = animInstance->GetAnimIndex(L"Teleport");
	animInstance->SetCurrentAnimTrack(idleIndex);
}
void MageHitState::Tick()
{
	runElapsed += TIMER->GetDeltaTime();
	if (bMove)
	{
		// 이동
		Vec3 pos = dir * Vec3(0.1f, 0.0f, 0.1f);
		m_pOwner.lock()->AddPosition(pos);
		if (runElapsed > 1.3f)
		{
			bMove = false;
			runElapsed = 0.0f;
			targetYaw = atan2f(-dir.x, -dir.z);
		}
	}
	else
	{
		// 회전 
		rotateElapsed += TIMER->GetDeltaTime();

		if (rotateElapsed < 0.4f)
		{
			//Vec3 currentRot = m_pOwner.lock()->GetRotation();
			//float currentYaw = currentRot.y;
			//float angleDiff = targetYaw - currentYaw;
			//while (angleDiff > DD_PI)  angleDiff -= DD_PI * 2;
			//while (angleDiff < -DD_PI) angleDiff += DD_PI * 2;
			//float smoothedYaw = currentRot.y + angleDiff * 8.0f * TIMER->GetDeltaTime();
			//currentRot.y = smoothedYaw;
			//m_pOwner.lock()->SetRotation(currentRot);
		}
		else
		{
			rotateElapsed = 0.0f;
			bMove = true;
			End();
		}
	}
}
void MageHitState::End()
{
	// 기본 state 세팅
	m_bOnPlaying = false;
}
void MageHitState::SetDirection(Vec3 _playerPos)
{
	dir = m_pOwner.lock()->GetPosition() - _playerPos;
	dir.Normalize();
}

MageAttackState::MageAttackState(weak_ptr<AActor> _pOwner) : StateBase(ENEMY_S_ATTACK)
{
	m_pOwner = _pOwner;
	m_bCanInterrupt = false;

	attack = make_shared<MageAttackStart>(m_pOwner.lock());
	runaway = make_shared<MageRunaway>(m_pOwner.lock());
	disappear = make_shared<MageDisappearState>(m_pOwner.lock());
	appear = make_shared<MageAppearState>(m_pOwner.lock());
}
void MageAttackState::Enter()
{
	// 기본 init
	m_bOnPlaying = true;

	attack->SetTarget(m_pTarget);
	attack->Enter();
}
void MageAttackState::Tick()
{
	switch (currentPhase)
	{
	case AttackPhase::Attack:
		attack->Tick();
		if (!attack->IsPlaying())
		{
			runaway->SetDirection(m_pTarget.lock()->GetPosition());
			runaway->Enter();
			currentPhase = AttackPhase::Runaway;
		}
		break;
	case AttackPhase::Runaway:
		runaway->Tick();
		if (!runaway->IsPlaying())
		{
			disappear->Enter();
			currentPhase = AttackPhase::Disappear;
		}
		break;
	case AttackPhase::Disappear:
		disappear->Tick();
		if (!disappear->IsPlaying())
		{
			currentPhase = AttackPhase::Wait;
			//m_pOwner.lock()->SetScale(Vec3(1.0f, 1.0f, 1.0f));
			m_pOwner.lock()->m_bCollision = false;
			m_pOwner.lock()->m_bRender = false;
			m_pOwner.lock()->GetShapeComponent()->m_bVisible = false;

		}
		break;
	case AttackPhase::Wait:
		// 일정 시간 흐르면 appear 실행
		disElapsed += TIMER->GetDeltaTime();
		if (disElapsed > 3.0f)
		{
			Vec3 oppositLook = m_pOwner.lock()->GetPosition() - m_pTarget.lock()->GetPosition();
			oppositLook.y = 0.0f;
			oppositLook.Normalize();
			destination = m_pTarget.lock()->GetPosition() + oppositLook * Vec3(15.0f, 0.0f, 15.0f);
			m_pOwner.lock()->SetPosition(destination);
			currentPhase = AttackPhase::Appear;
			dir = m_pTarget.lock()->GetPosition() - m_pOwner.lock()->GetPosition();
			appear->SetDirection(dir);
			appear->Enter();
			disElapsed = 0.0f;
		}
		break;
	case AttackPhase::Appear:
		appear->Tick();
		if (!appear->IsPlaying())
		{
			currentPhase = AttackPhase::Done;
		}
		break;
	case AttackPhase::Done:
		End();
		break;
	}
}
void MageAttackState::End()
{
	// 기본 state 세팅
	m_bOnPlaying = false;
	currentPhase = AttackPhase::Attack;
}
void MageAttackState::SetTarget(weak_ptr<AActor> _player)
{
	m_pTarget = _player;
}
void MageAttackState::SetDirection()
{
	auto playerPos = m_pTarget.lock()->GetPosition();
	auto handSocket = m_pOwner.lock()->GetMeshComponent()->GetChildByName(L"RightHandSocket");
	float y = playerPos.y - handSocket->GetWorldPosition().y;

	dir = playerPos - m_pOwner.lock()->GetPosition();
	dir.y = y;
	dir.Normalize();
}

#pragma region Attack-sub-state
MageAttackStart::MageAttackStart(weak_ptr<AActor> _pOwner) : StateBase(ENEMY_S_ATTACK)
{
	m_pOwner = _pOwner;
	m_bCanInterrupt = false;

	auto animInstance = m_pOwner.lock()->GetMeshComponent<USkinnedMeshComponent>()->GetAnimInstance();
	int index = animInstance->GetAnimIndex(L"Shoot");
	animInstance->AddEvent(index, 37, [this]() {
		this->Throw();
		});
}
void MageAttackStart::Enter()
{
	// 기본 state 세팅
	m_bOnPlaying = true;
	// 애니메이션 Idle 플레이
	auto animInstance = m_pOwner.lock()->GetMeshComponent<USkinnedMeshComponent>()->GetAnimInstance();
	int index = animInstance->GetAnimIndex(L"Shoot");
	animInstance->PlayOnce(index);
	// 회전
	//targetYaw = atan2f(dir.x, dir.z);
}
void MageAttackStart::Tick()
{
	auto animInstance = m_pOwner.lock()->GetMeshComponent<USkinnedMeshComponent>()->GetAnimInstance();
	if (!animInstance->m_bOnPlayOnce)
	{
		// 애니메이션 종료
		End();
	}

	//// rotation
	//Vec3 currentRot = m_pOwner.lock()->GetRotation();
	//float currentYaw = currentRot.y;

	//float angleDiff = targetYaw - currentYaw;
	//while (angleDiff > DD_PI)  angleDiff -= DD_PI * 2;
	//while (angleDiff < -DD_PI) angleDiff += DD_PI * 2;

	//float smoothedYaw = currentRot.y + angleDiff * 8.0f * TIMER->GetDeltaTime();

	//currentRot.y = smoothedYaw;
	//m_pOwner.lock()->SetRotation(currentRot);
}
void MageAttackStart::End()
{
	// 기본 state 세팅
	m_bOnPlaying = false;
}
void MageAttackStart::Throw()
{
	// action
	SetDirection();

	auto handSocket = m_pOwner.lock()->GetMeshComponent()->GetChildByName(L"RightHandSocket");
	Vec3 pos = handSocket->GetWorldPosition();
	PROJECTILE->ActivateOne(ProjectileType::MagicBall, pos, dir);
}
void MageAttackStart::SetTarget(weak_ptr<AActor> _target)
{
	m_pTarget = _target;
}
void MageAttackStart::SetDirection()
{
	auto playerPos = m_pTarget.lock()->GetPosition();
	auto handSocket = m_pOwner.lock()->GetMeshComponent()->GetChildByName(L"RightHandSocket");
	float y = playerPos.y - handSocket->GetWorldPosition().y;

	dir = playerPos - m_pOwner.lock()->GetPosition();
	dir.y = y;
	dir.Normalize();
}

MageRunaway::MageRunaway(weak_ptr<AActor> _pOwner) : StateBase(ENEMY_S_ATTACK)
{
	m_pOwner = _pOwner;
	m_bCanInterrupt = false;
}
void MageRunaway::Enter()
{
	// 기본 state 세팅
	m_bOnPlaying = true;
	// 애니메이션 Idle 플레이
	auto animInstance = m_pOwner.lock()->GetMeshComponent<USkinnedMeshComponent>()->GetAnimInstance();
	int idleIndex = animInstance->GetAnimIndex(L"Spiral");
	animInstance->SetCurrentAnimTrack(idleIndex);
}
void MageRunaway::Tick()
{
	elapsed += TIMER->GetDeltaTime();
	if (elapsed > 1.5f)
	{
		elapsed = 0.0f;
		End();
	}

	// 돌면서 이동
	dir.Normalize();
	m_pOwner.lock()->AddPosition(dir * 0.1f);

}
void MageRunaway::End()
{
	// 기본 state 세팅
	m_bOnPlaying = false;
}
void MageRunaway::SetDirection(Vec3 _targetPos)
{
	dir = m_pOwner.lock()->GetPosition() - _targetPos;
}
#pragma endregion


MageDieState::MageDieState(weak_ptr<AActor> _pOwner) : StateBase(ENEMY_S_DEATH)
{
	m_pOwner = _pOwner;
	m_bCanInterrupt = false;
}

void MageDieState::Enter()
{
	// 기본 state 세팅
	m_bOnPlaying = true;
	// 애니메이션 Idle 플레이
	auto animInstance = m_pOwner.lock()->GetMeshComponent<USkinnedMeshComponent>()->GetAnimInstance();
	int index = animInstance->GetAnimIndex(L"Death");
	animInstance->PlayOnce(index);
}

void MageDieState::Tick()
{
	auto animInstance = m_pOwner.lock()->GetMeshComponent<USkinnedMeshComponent>()->GetAnimInstance();
	if (!animInstance->m_bOnPlayOnce)
	{
		// 애니메이션 종료
		End();
	}

	float frameTime = animInstance->GetTotalFrame();
	frameTime /= 30;
	m_fDissolveTimer += TIMER->GetDeltaTime();
	float t = m_fDissolveTimer / frameTime;
	auto comp = m_pOwner.lock()->GetMeshComponent<USkinnedMeshComponent>();
	ApplyDissolveToAllMaterials(comp, t);
}

void MageDieState::ApplyDissolveToAllMaterials(shared_ptr<class UMeshComponent> _comp, float _time)
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

void MageDieState::End()
{
	// 기본 state 세팅
	m_bOnPlaying = false;
}

