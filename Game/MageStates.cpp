#include "pch.h"
#include "MageStates.h"
#include "USkinnedMeshComponent.h"
#include "UAnimInstance.h"
#include "AActor.h"

#include "Timer.h"
#include "ProjectileManager.h"
#include "EffectManager.h"

// TEMP
#include "Input.h"
#include "Sound.h"

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

	//targetYaw = targetYaw = atan2f(dir.x, dir.z);
	//Vec3 currentRot = m_pOwner.lock()->GetRotation();
	//currentRot.y = targetYaw;
	//m_pOwner.lock()->SetRotation(currentRot);

	m_pOwner.lock()->m_bCollision = true;
	SetMeshVisible();
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

void MageAppearState::SetMeshVisible()
{
	//auto body = m_pOwner.lock()->GetMeshComponent();
	//auto weapon = m_pOwner.lock()->GetMeshComponent()->GetChildByName(L"Chain");
	//auto chain = m_pOwner.lock()->GetMeshComponent()->GetChildByName(L"Weapon");

	//body->SetVisible(true);
	//weapon->SetVisible(true);
	//chain->SetVisible(true);

	m_pOwner.lock()->SetScale(Vec3(0.5f, 0.5f, 0.5f));
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
	if (lowerElapsed < 1.3f)
	{
		auto hand = m_pOwner.lock()->GetMeshComponent()->GetChildByName(L"RightHandSocket");
		Vec3 pos = hand->GetWorldPosition();
		//EFFECT->PlayDustBurst(pos, 10.f, .1f);
		EFFECT->PlayEffect(EEffectType::Dust, pos, 0, Vec3(0.0f, 0.0f, 0.0f), 1.3f);
	}
	if (lowerElapsed > 1.6f)
	{
		lowerElapsed = 0.0f;
	}
	auto animInstance = m_pOwner.lock()->GetMeshComponent<USkinnedMeshComponent>()->GetAnimInstance();
	if (!animInstance->m_bOnPlayOnce)
	{
		SetAllMeshInvisible(m_pOwner.lock()->GetMeshComponent());
		// 애니메이션 종료
		End();
	}
}
void MageDisappearState::End()
{
	// 기본 state 세팅
	m_bOnPlaying = false;
}

void MageDisappearState::SetAllMeshInvisible(const shared_ptr<UMeshComponent>& _mesh)
{
	//_mesh->SetVisible(false);
	//auto children = _mesh->GetChildren();
	//for (auto child : children)
	//{
	//	SetAllMeshInvisible(child);
	//}

	m_pOwner.lock()->SetScale(Vec3(0, 0, 0));
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
	if (bMove && !bStaticMage)
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

	auto animInstance = m_pOwner.lock()->GetMeshComponent<USkinnedMeshComponent>()->GetAnimInstance();
	originSpped = animInstance->m_fAnimPlayRate;
	//animInstance->m_fAnimPlayRate = 25.0f;
}
void MageAttackState::Tick()
{
	switch (currentPhase)
	{
	case AttackPhase::Attack:
		attack->Tick();
		if (!attack->IsPlaying())
		{
			if (!bStaticMage)
			{
				runaway->SetDirection(m_pTarget.lock()->GetPosition());
				runaway->Enter();
				currentPhase = AttackPhase::Runaway;
			}
			else
			{
				// 애니메이션 Idle 플레이
				auto animInstance = m_pOwner.lock()->GetMeshComponent<USkinnedMeshComponent>()->GetAnimInstance();
				int idleIndex = animInstance->GetAnimIndex(L"Idle2");
				animInstance->SetCurrentAnimTrack(idleIndex);
				currentPhase = AttackPhase::StandStill;

			}
		}
		break;
	case AttackPhase::StandStill:
	{
		waitElapsed += TIMER->GetDeltaTime();
		if (waitElapsed > 2.0f)
		{
			waitElapsed = 0.0f;
			attack->Enter();
			currentPhase = AttackPhase::Attack;
		}
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
			m_pOwner.lock()->m_bCollision = false;
			m_pOwner.lock()->GetShapeComponent()->m_bVisible = false;

		}
		break;
	case AttackPhase::Wait:
		// 일정 시간 흐르면 appear 실행
		disElapsed += TIMER->GetDeltaTime();
		if (disElapsed > 2.0f)
		{
			// 변경 필요 
			// 위치 특정한 지점 미리 잡아놓고 거기서만 이동하도록
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

	auto animInstance = m_pOwner.lock()->GetMeshComponent<USkinnedMeshComponent>()->GetAnimInstance();
	animInstance->m_fAnimPlayRate = originSpped;
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
	auto hand = m_pOwner.lock()->GetMeshComponent()->GetChildByName(L"RightHandSocket");
	Vec3 pos = hand->GetWorldPosition();
	//EFFECT->PlayDustBurst(pos, 10.f, .1f);
	EFFECT->PlayEffect(EEffectType::Dust, pos, 0, Vec3(0.0f, 0.0f, 0.0f), 2.3f);

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
	pos.y -= 1.0f;
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
	auto hand = m_pOwner.lock()->GetMeshComponent()->GetChildByName(L"RightHandSocket");
	Vec3 pos = hand->GetWorldPosition();
	//EFFECT->PlayDustBurst(pos, 10.f, .1f);
	EFFECT->PlayEffect(EEffectType::Dust, pos, 0, Vec3(0.0f, 0.0f, 0.0f), 2.0f);

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

	Vec3 playerPos =m_pPlayer.lock()->GetPosition();
	playerPos.y += 1.5f;
	Vec3 soulDirection = playerPos - m_pOwner.lock()->GetPosition();
	soulDirection.Normalize();
	EFFECT->PlayEffect(EEffectType::Soul, m_pOwner.lock()->GetPosition(), 0, soulDirection, 1.0f, playerPos);

	SOUND->GetPtr(ESoundType::Enemy_Damaged)->PlayEffect2D();
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

