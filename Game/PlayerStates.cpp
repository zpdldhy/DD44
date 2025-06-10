#include "pch.h"
#include "PlayerStates.h"
#include "AActor.h"
#include "USkinnedMeshComponent.h"
#include "UAnimInstance.h"
#include "Sound.h"
#include "EffectManager.h"
#include "Timer.h"


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

	SOUNDMANAGER->GetPtr(ESoundType::Walk)->Play2D();

	m_fDustTimer = 0.0f;
}

void PlayerWalkState::Tick()
{
	auto owner = m_pOwner.lock();
	if (!owner)
		return;

	float deltaTime = TIMER->GetDeltaTime();
	m_fDustTimer += deltaTime;

	// 걷는 도중 0.5초마다 먼지 생성
	if (m_fDustTimer >= 0.5f)
	{
		Vec3 pos = owner->GetPosition();
		Vec3 dir = owner->GetLook() * -1.f;
		pos.y += 0.3f;

		Vec3 dustVelocity = dir * 0.0f + Vec3(0, 3.0f, 0);
		EFFECT->PlayEffect(EEffectType::Dust, pos, 0.0f, dustVelocity);

		m_fDustTimer = 0.0f;
	}
}

void PlayerWalkState::End()
{
	// 기본 state 세팅
	m_bOnPlaying = false;
	SOUNDMANAGER->GetPtr(ESoundType::Walk)->Stop();
	m_fDustTimer = 0.0f;
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
	SOUNDMANAGER->GetPtr(ESoundType::Slash)->PlayEffect2D();

	// 애니메이션 Attack 플레이
	auto animInstance = m_pOwner.lock()->GetMeshComponent<USkinnedMeshComponent>()->GetAnimInstance();
	int attackIndex = animInstance->GetAnimIndex(L"Slash_Light_R_new");
	animInstance->PlayOnce(attackIndex);
}

void PlayerAttackState::Tick()
{
	auto animInstance = m_pOwner.lock()->GetMeshComponent<USkinnedMeshComponent>()->GetAnimInstance();
	if (!animInstance->m_bOnPlayOnce)
	{
		// 애니메이션 종료
		End();
	}
}

void PlayerAttackState::End()
{
	// 기본 state 세팅
	m_bOnPlaying = false;
}

void PlayerAttackState::SetComponent(shared_ptr<UMeshComponent> _sword, shared_ptr<UMeshComponent> _hand, shared_ptr<UMeshComponent> _back)
{
	m_pSword = _sword;
	m_pHandSocket = _hand;
	m_pBackSocket = _back;
}

PlayerHitState::PlayerHitState(weak_ptr<AActor> _pOwner) : StateBase(PLAYER_S_HIT)
{
	m_pOwner = _pOwner;
	m_bCanInterrupt = false;
}

void PlayerHitState::Enter()
{
	// 기본 state 세팅
	m_bOnPlaying = true;
	m_bPlayNext = true;

	// 애니메이션 Attack 플레이
	auto animInstance = m_pOwner.lock()->GetMeshComponent<USkinnedMeshComponent>()->GetAnimInstance();
	int index = animInstance->GetAnimIndex(L"Hit_back");
	animInstance->PlayOnce(index);

	//Sound
	SOUNDMANAGER->GetPtr(ESoundType::Hit)->PlayEffect2D();

}

void PlayerHitState::Tick()
{
	auto animInstance = m_pOwner.lock()->GetMeshComponent<USkinnedMeshComponent>()->GetAnimInstance();
	if (!animInstance->m_bOnPlayOnce)
	{
		// 1번만 다음 애니메이션 재생
		if (m_bPlayNext)
		{
			int index = animInstance->GetAnimIndex(L"Hit_Recover");
			animInstance->PlayOnce(index);
			m_bPlayNext = false;
			return;
		}
		
		End();
	}
}

void PlayerHitState::End()
{
	// 기본 state 세팅
	m_bOnPlaying = false;
}

PlayerDieState::PlayerDieState(weak_ptr<AActor> _pOwner) : StateBase(PLAYER_S_DEATH)
{
	m_pOwner = _pOwner;
	m_bCanInterrupt = false;
}

void PlayerDieState::Enter()
{
	// 기본 state 세팅
	m_bOnPlaying = true;
	m_bPlayNext = true;

	// 애니메이션 Attack 플레이
	auto animInstance = m_pOwner.lock()->GetMeshComponent<USkinnedMeshComponent>()->GetAnimInstance();
	int index = animInstance->GetAnimIndex(L"Hit_back");
	animIndex = animInstance->GetAnimIndex(L"Hit_idle");
	animInstance->PlayOnce(index);

	// 사운드
	SOUNDMANAGER->GetPtr(ESoundType::Die)->PlayEffect2D();
}

void PlayerDieState::Tick()
{
	auto animInstance = m_pOwner.lock()->GetMeshComponent<USkinnedMeshComponent>()->GetAnimInstance();
	if (!animInstance->m_bOnPlayOnce)
	{
		// 다음 애니메이션 재생
		if (m_bPlayNext)
		{
			animInstance->SetCurrentAnimTrack(animIndex);
			m_bPlayNext = false;
			return;
		}
	}
	// end 하지 않음. 게임 종료 . 이처리를 어디서

}

void PlayerDieState::End()
{
	// 기본 state 세팅
	m_bOnPlaying = false;
}

PlayerRollState::PlayerRollState(weak_ptr<AActor> _pOwner) : StateBase(PLAYER_S_ROLL)
{
	m_pOwner = _pOwner;
	m_bCanInterrupt = false;
}

void PlayerRollState::Enter()
{
	// 기본 state 세팅
	m_bOnPlaying = true;
	m_iDustSpawnCount = 0;
	m_fDustSpawnTimer = 0.0f;

	// 애니메이션 Attack 플레이
	auto animInstance = m_pOwner.lock()->GetMeshComponent<USkinnedMeshComponent>()->GetAnimInstance();
	int index = animInstance->GetAnimIndex(L"Roll_heavy");
	animInstance->PlayOnce(index);

	// 사운드
	SOUNDMANAGER->GetPtr(ESoundType::Dash)->PlayEffect2D();

}

void PlayerRollState::Tick()
{
	auto owner = m_pOwner.lock();
	if (!owner)
		return;

	// 먼지 반복 생성
	m_fDustSpawnTimer += TIMER->GetDeltaTime();

	if (m_iDustSpawnCount < 3 && m_fDustSpawnTimer >= 0.17f)
	{
		Vec3 pos = owner->GetPosition();
		Vec3 dir = owner->GetLook() * -1.f;
		pos.y += 0.3f;

		Vec3 dustVelocity = dir * 20.0f + Vec3(0, 5.0f, 0);
		EFFECT->PlayEffect(EEffectType::Feather, pos, 10.0f, dustVelocity);

		m_fDustSpawnTimer = 0.0f;
		m_iDustSpawnCount++;
	}

	auto animInstance = m_pOwner.lock()->GetMeshComponent<USkinnedMeshComponent>()->GetAnimInstance();
	if (!animInstance->m_bOnPlayOnce)
	{
		End();
	}
}

void PlayerRollState::End()
{
	// 기본 state 세팅
	m_bOnPlaying = false;
}

PlayerShootState::PlayerShootState(weak_ptr<AActor> _pOwner) : StateBase(PLAYER_S_SHOOT)
{
	m_pOwner = _pOwner;
	m_bCanInterrupt = false;
}

void PlayerShootState::Enter()
{
	// 기본 state 세팅
	m_bOnPlaying = true;

	// 애니메이션 Attack 플레이
	auto animInstance = m_pOwner.lock()->GetMeshComponent<USkinnedMeshComponent>()->GetAnimInstance();
	int index = animInstance->GetAnimIndex(L"Arrow");
	animInstance->PlayOnce(index);
}

void PlayerShootState::Tick()
{
	auto animInstance = m_pOwner.lock()->GetMeshComponent<USkinnedMeshComponent>()->GetAnimInstance();
	if (!animInstance->m_bOnPlayOnce)
	{
		End();
	}
}

void PlayerShootState::End()
{
	// 기본 state 세팅
	m_bOnPlaying = false;
}
