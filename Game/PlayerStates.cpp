#include "pch.h"
#include "PlayerStates.h"
#include "TPlayer.h"
#include "USkinnedMeshComponent.h"
#include "UAnimInstance.h"
#include "Sound.h"
#include "EffectManager.h"
#include "Timer.h"
#include "CollisionManager.h"
#include "ObjectManager.h"
#include "ProjectileManager.h"

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

	// �ȴ� ���� 0.5�ʸ��� ���� ����
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
	// �⺻ state ����
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
	// �⺻ state ����
	m_bOnPlaying = true;
	SOUNDMANAGER->GetPtr(ESoundType::Slash)->PlayEffect2D();

	// �ִϸ��̼� Attack �÷���
	auto animInstance = m_pOwner.lock()->GetMeshComponent<USkinnedMeshComponent>()->GetAnimInstance();
	int attackIndex = animInstance->GetAnimIndex(L"Slash_Light_R_new");
	animInstance->PlayOnce(attackIndex);
}
void PlayerAttackState::Tick()
{
	auto animInstance = m_pOwner.lock()->GetMeshComponent<USkinnedMeshComponent>()->GetAnimInstance();
	if (!animInstance->m_bOnPlayOnce)
	{
		// �ִϸ��̼� ����
		End();
	}
}
void PlayerAttackState::End()
{
	// �⺻ state ����
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
	// �⺻ state ����
	m_bOnPlaying = true;
	m_bPlayNext = true;

	// �ִϸ��̼� Attack �÷���
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
		// 1���� ���� �ִϸ��̼� ���
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
	// �⺻ state ����
	m_bOnPlaying = false;
}

PlayerDieState::PlayerDieState(weak_ptr<AActor> _pOwner) : StateBase(PLAYER_S_DEATH)
{
	m_pOwner = _pOwner;
	m_bCanInterrupt = false;
}
void PlayerDieState::Enter()
{
	// �⺻ state ����
	m_bOnPlaying = true;
	m_bPlayNext = true;

	// �ִϸ��̼� Attack �÷���
	auto animInstance = m_pOwner.lock()->GetMeshComponent<USkinnedMeshComponent>()->GetAnimInstance();
	int index = animInstance->GetAnimIndex(L"Hit_back");
	animIndex = animInstance->GetAnimIndex(L"Hit_idle");
	animInstance->PlayOnce(index);

	// ����
	SOUNDMANAGER->GetPtr(ESoundType::Die)->PlayEffect2D();
}
void PlayerDieState::Tick()
{
	auto animInstance = m_pOwner.lock()->GetMeshComponent<USkinnedMeshComponent>()->GetAnimInstance();
	if (!animInstance->m_bOnPlayOnce)
	{
		// ���� �ִϸ��̼� ���
		if (m_bPlayNext)
		{
			animInstance->SetCurrentAnimTrack(animIndex);
			m_bPlayNext = false;
			return;
		}
	}
	// end ���� ����. ���� ���� . ��ó���� ���

}
void PlayerDieState::End()
{
	// �⺻ state ����
	m_bOnPlaying = false;
}

PlayerRollState::PlayerRollState(weak_ptr<AActor> _pOwner) : StateBase(PLAYER_S_ROLL)
{
	m_pOwner = _pOwner;
	m_bCanInterrupt = false;
}
void PlayerRollState::Enter()
{
	// �⺻ state ����
	m_bOnPlaying = true;
	m_iDustSpawnCount = 0;
	m_fDustSpawnTimer = 0.0f;

	// �ִϸ��̼� Attack �÷���
	auto animInstance = m_pOwner.lock()->GetMeshComponent<USkinnedMeshComponent>()->GetAnimInstance();
	int index = animInstance->GetAnimIndex(L"Roll_heavy");
	animInstance->PlayOnce(index);

	// ����
	SOUNDMANAGER->GetPtr(ESoundType::Dash)->PlayEffect2D();

}
void PlayerRollState::Tick()
{
	auto owner = m_pOwner.lock();
	if (!owner)
		return;

	// ���� �ݺ� ����
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
	// �⺻ state ����
	m_bOnPlaying = false;
}

PlayerShootState::PlayerShootState(weak_ptr<AActor> _pOwner) : StateBase(PLAYER_S_SHOOT)
{
	m_pOwner = _pOwner;
	m_bCanInterrupt = true;

	// sub-state
	start = make_shared<PlayerShootStart>(m_pOwner);
	shoot = make_shared<PlayerShoot>(m_pOwner);
}
void PlayerShootState::Enter()
{
	// �⺻ state ����
	m_bOnPlaying = true;

	start->Enter();
	shoot->CheckShootCount(bCanShoot);
}
void PlayerShootState::Tick()
{
	CheckMouse();

	switch (currentPhase)
	{
	case ShootPhase::Start:
		Rotate();
		if (bEnd)
		{
			start->End();
			currentPhase = ShootPhase::Done;
			break;
		}
		start->Tick();
		if (!start->IsPlaying())
		{
			currentPhase = ShootPhase::Aiming;
		}
		break;
	case ShootPhase::Aiming:
		Rotate();
		if (bEnd)
		{
			currentPhase = ShootPhase::Shoot;
			shoot->Enter();
			break;
		}

		break;
	case ShootPhase::Shoot:
		shoot->Tick();
		if (!shoot->IsPlaying())
		{
			currentPhase = ShootPhase::Done;
			break;
		}
		break;
	case ShootPhase::Done:
		End();
		break;
	}
}
void PlayerShootState::End()
{
	// �⺻ state ����
	m_bOnPlaying = false;

	// reset
	currentPhase = ShootPhase::Start;
	auto animInstance = m_pOwner.lock()->GetMeshComponent<USkinnedMeshComponent>()->GetAnimInstance();
	animInstance->m_bPlay = true;
	animInstance->m_bOnPlayOnce = false;
	bEnd = false;

}
void PlayerShootState::Rotate()
{
	//dir = m_pTarget.lock()->GetPosition() - m_pOwner.lock()->GetPosition();
	float targetYaw = atan2f(dir.x, dir.z);
	Vec3 currentRot = m_pOwner.lock()->GetRotation();
	float currentYaw = currentRot.y;

	// ���� ���� ���
	float angleDiff = targetYaw - currentYaw;
	while (angleDiff > DD_PI)  angleDiff -= DD_PI * 2;
	while (angleDiff < -DD_PI) angleDiff += DD_PI * 2;

	// Lerp ���
	float smoothedYaw = currentRot.y + angleDiff * 8.0f * TIMER->GetDeltaTime();

	currentRot.y = smoothedYaw;
	m_pOwner.lock()->SetRotation(currentRot);
}
void PlayerShootState::CheckMouse()
{
	MouseRay ray;
	ray.Click();

	auto playerPos = m_pOwner.lock()->GetPosition();
	playerPos.y += 1.f;
	Vec3 inter;

	Collision::GetIntersection(ray, playerPos, Vec3(0.f, 1.f, 0.f), inter);
	dir = inter - m_pOwner.lock()->GetPosition();

}
void PlayerShootState::CheckShootCount(bool _able)
{
	bCanShoot = _able;
}

PlayerShootStart::PlayerShootStart(weak_ptr<AActor> _pOwner) : StateBase(PLAYER_S_SHOOT)
{
	m_pOwner = _pOwner;
	m_bCanInterrupt = true;
}
void PlayerShootStart::Enter()
{
	// �⺻ state ����
	m_bOnPlaying = true;

	// �ִϸ��̼� Attack �÷���
	auto animInstance = m_pOwner.lock()->GetMeshComponent<USkinnedMeshComponent>()->GetAnimInstance();
	int index = animInstance->GetAnimIndex(L"Arrow");
	animInstance->PlayOnce(index);

	// pr timer�� ���� ( �ð� ���߱� ������ anim key event �߰��� �ٲ��� ) 

}
void PlayerShootStart::Tick()
{
	elapsed += TIMER->GetDeltaTime();
	if (elapsed > 0.7f)
	{
		auto animInstance = m_pOwner.lock()->GetMeshComponent<USkinnedMeshComponent>()->GetAnimInstance();
		animInstance->m_bPlay = false;
		End();
	}
}
void PlayerShootStart::End()
{
	// �⺻ state ����
	m_bOnPlaying = false;
	// Reset
	elapsed = 0.0f;
}

PlayerShoot::PlayerShoot(weak_ptr<AActor> _pOwner) : StateBase(PLAYER_S_SHOOT)
{
	m_pOwner = _pOwner;
	m_bCanInterrupt = true;

	// AnimSetting
	auto animInstance = m_pOwner.lock()->GetMeshComponent<USkinnedMeshComponent>()->GetAnimInstance();
	int index = animInstance->GetAnimIndex(L"Arrow");

	// ���� ���� ���ο� ���� �ٸ� �̺�Ʈ �߰�
	// Anim�� key �ִ� �� �� ������ �� �غ����� �� Enter�� ��� �����ڳ� 
	// �׸��� ��� �ְ�. 
	animInstance->AddEvent(index, 22, [this]() {
		this->CanShoot();
		});
}
void PlayerShoot::Enter()
{
	// �⺻ state ����
	m_bOnPlaying = true;

	// �ִϸ��̼� Attack �÷���
	auto animInstance = m_pOwner.lock()->GetMeshComponent<USkinnedMeshComponent>()->GetAnimInstance();
	animInstance->m_bPlay = true;
}
void PlayerShoot::Tick()
{
	auto animInstance = m_pOwner.lock()->GetMeshComponent<USkinnedMeshComponent>()->GetAnimInstance();
	if (!animInstance->m_bOnPlayOnce)
	{
		// �ִϸ��̼� ����
		End();
	}
}
void PlayerShoot::End()
{
	// �⺻ state ����
	m_bOnPlaying = false;
}
void PlayerShoot::CheckShootCount(bool _able)
{
	bCanShoot = _able;
}
void PlayerShoot::CanShoot()
{
	if (!bCanShoot) { return; }
	Profiler p("CanShoot");
	// ����ü �߻�
	auto  hand = m_pOwner.lock()->GetMeshComponent()->GetChildByName(L"LeftHandSocket");
	Vec3 pos = hand->GetWorldPosition();

	Vec3 look = m_pOwner.lock()->GetLook();
	PROJECTILE->ActivateOne(ProjectileType::PlayerArrow, pos, look);

	dynamic_pointer_cast<TPlayer>(m_pOwner.lock())->DecArrowCount();
}

PlayerClimbState::PlayerClimbState(weak_ptr<AActor> _pOwner) : StateBase(PLAYER_S_CLIMB)
{
	m_pOwner = _pOwner;
	// ��ٸ� ���� �� ���ݴ��ϸ� ��� ���� ? 
	m_bCanInterrupt = false;

	finish = make_shared<PlayerClimbFinish>(m_pOwner);
}
void PlayerClimbState::Enter()
{
	// �⺻ state ����
	m_bOnPlaying = true;

	// �ִϸ��̼� Attack �÷���
	auto animInstance = m_pOwner.lock()->GetMeshComponent<USkinnedMeshComponent>()->GetAnimInstance();
	int attackIndex = animInstance->GetAnimIndex(L"Climbing_ladder");
	animInstance->SetCurrentAnimTrack(attackIndex);

	// �߷� ����
	m_pOwner.lock()->GetPhysicsComponent()->SetWeight(0.f);
}
void PlayerClimbState::Tick()
{
	float targetYaw = atan2f(ladderDir.x, ladderDir.z);
	Vec3 currentRot = m_pOwner.lock()->GetRotation();
	currentRot.y = targetYaw;
	m_pOwner.lock()->SetRotation(currentRot);

	auto animInstance = m_pOwner.lock()->GetMeshComponent<USkinnedMeshComponent>()->GetAnimInstance();
	switch (currentPhase)
	{
	case ClimbPhase::Playing:
	{
		if (isMoving)
		{
			animInstance->m_bPlay = true;
		}
		else
		{
			animInstance->m_bPlay = false;
		}

		if (isFinish)
		{
			currentPhase = ClimbPhase::Finish;
			finish->SetLadderDir(ladderDir);
			finish->Enter();
		}
	}
	break;
	case ClimbPhase::Finish:
	{
		finish->Tick();
		if (!finish->IsPlaying())
		{
			currentPhase = ClimbPhase::Done;
			End();
		}
	}
	break;
	case ClimbPhase::Done:
		End();
		break;
	}

}
void PlayerClimbState::End()
{
	// �⺻ state ����
	m_bOnPlaying = false;
	isFinish = false;
	currentPhase = ClimbPhase::Playing;
}
void PlayerClimbState::CheckMove(bool _isMoving)
{
	isMoving = _isMoving;
	auto animInstance = m_pOwner.lock()->GetMeshComponent<USkinnedMeshComponent>()->GetAnimInstance();
	animInstance->m_bPlay = isMoving;
}

void PlayerClimbState::SetLadderDir(Vec3 _dir)
{
	ladderDir = _dir;
}

PlayerClimbFinish::PlayerClimbFinish(weak_ptr<AActor> _pOwner) : StateBase(PLAYER_S_CLIMB)
{
	m_pOwner = _pOwner;
	m_bCanInterrupt = false;
}
void PlayerClimbFinish::Enter()
{
	// �⺻ state ����
	m_bOnPlaying = true;

	// �ִϸ��̼� Attack �÷���
	auto animInstance = m_pOwner.lock()->GetMeshComponent<USkinnedMeshComponent>()->GetAnimInstance();
	int index = animInstance->GetAnimIndex(L"Climbing_off_ladder_top");
	animInstance->m_bPlay = true;
	animInstance->PlayOnce(index);
}
void PlayerClimbFinish::Tick()
{
	// AddPosition ����
	ladderDir.Normalize();
	Vec3 look = ladderDir;
	Vec3 up = m_pOwner.lock()->GetUp();

	float offset = 1.0f;
	Vec3 moveDir = look * 1.0f + up * 0.5f;

	moveDir.Normalize();

	m_pOwner.lock()->AddPosition(moveDir * 0.05);

	auto animInstance = m_pOwner.lock()->GetMeshComponent<USkinnedMeshComponent>()->GetAnimInstance();
	if (!animInstance->m_bOnPlayOnce)
	{
		// �ִϸ��̼� ����
		m_pOwner.lock()->GetPhysicsComponent()->SetWeight(1.f);
		dynamic_pointer_cast<TPlayer>(m_pOwner.lock())->StopClimbing();
		End();
	}
}
void PlayerClimbFinish::End()
{
	// �⺻ state ����
	m_bOnPlaying = false;
}

void PlayerClimbFinish::SetLadderDir(Vec3 _dir)
{
	ladderDir = _dir;
}
