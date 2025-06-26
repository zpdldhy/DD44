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
#include "UMaterial.h"

PlayerEmptyState::PlayerEmptyState() : PlayerBaseState(EMPTY_STATE)
{
	
}
void PlayerEmptyState::Enter() {}
void PlayerEmptyState::Tick() {}
void PlayerEmptyState::End() {}

PlayerIdleState::PlayerIdleState(weak_ptr<AActor> _pOwner) : PlayerBaseState(PLAYER_S_IDLE)
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

PlayerWalkState::PlayerWalkState(weak_ptr<AActor> _pOwner) : PlayerBaseState(PLAYER_S_WALK)
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

	SOUND->GetPtr(ESoundType::Walk)->Play2D();

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
	SOUND->GetPtr(ESoundType::Walk)->Stop();
	m_fDustTimer = 0.0f;
}

PlayerAttackState::PlayerAttackState(weak_ptr<AActor> _pOwner) : PlayerBaseState(PLAYER_S_ATTACK)
{
	m_pOwner = _pOwner;
	m_bCanInterrupt = true;
}
void PlayerAttackState::Enter()
{
	// 기본 state 세팅
	m_bOnPlaying = true;
	SOUND->GetPtr(ESoundType::Slash)->PlayEffect2D();

	// Rotation
	CheckMouse();
	Rotate();

	// 애니메이션 Attack 플레이
	auto animInstance = m_pOwner.lock()->GetMeshComponent<USkinnedMeshComponent>()->GetAnimInstance();
	int attackIndex = animInstance->GetAnimIndex(L"Slash_Light_R_new");
	animInstance->m_fAnimPlayRate = 30.0f;
	animInstance->PlayOnce(attackIndex);

	// Sword
	auto back = m_pOwner.lock()->GetMeshComponent()->GetChildByName(L"BackSword");
	back->SetVisible(false);
	auto left = m_pOwner.lock()->GetMeshComponent()->GetChildByName(L"LeftSword");
	left->SetVisible(true);

	// Slash
	m_pOwner.lock()->GetMeshComponent()->GetChildByName(L"Slash")->SetVisible(true);

	// FX 세팅
	m_pSlashMaterial = m_pOwner.lock()->GetMeshComponent()->GetChildByName(L"Slash")->GetMaterial();
	m_bSlashPlaying = true;
	m_fSlashTime = 0.0f;
}
void PlayerAttackState::Tick()
{
	// FX
	Slash();

	//
	auto animInstance = m_pOwner.lock()->GetMeshComponent<USkinnedMeshComponent>()->GetAnimInstance();
	int nextAnim;
	bool oneEnd = false;
	if (!animInstance->m_bOnPlayOnce)
	{
		oneEnd = true;
		if (!m_bOnCombo)
		{
			currentPhase = AttackCombo::Done;
		}
	}
	if (!oneEnd) { return; }

	switch (currentPhase)
	{
	case AttackCombo::OnFirst:
	{
		// 앞으로 조금 이동
		reverse = true;
		m_bSlashPlaying = true;
		m_fSlashTime = 0.0f;
		CheckMouse();
		Rotate();
		Move();
		nextAnim = animInstance->GetAnimIndex(L"Slash_Light_L_new");
		animInstance->PlayOnce(nextAnim);

		// Sword
		auto left = m_pOwner.lock()->GetMeshComponent()->GetChildByName(L"LeftSword");
		left->SetVisible(false);
		auto right = m_pOwner.lock()->GetMeshComponent()->GetChildByName(L"RightSword");
		right->SetVisible(true);

		//
		m_pOwner.lock()->GetMeshComponent()->GetChildByName(L"Slash")->SetVisible(true);

		currentPhase = OnSecond;
		m_bOnCombo = false;

		SOUND->GetPtr(ESoundType::Slash_2)->PlayEffect2D();
	}
	break;
	case AttackCombo::OnSecond:
	{
		reverse = false;
		m_bSlashPlaying = true;
		m_fSlashTime = 0.0f;
		CheckMouse();
		Rotate();
		Move();
		nextAnim = animInstance->GetAnimIndex(L"Slash_Light_R_new");
		animInstance->PlayOnce(nextAnim);

		// Sword
		auto left = m_pOwner.lock()->GetMeshComponent()->GetChildByName(L"LeftSword");
		left->SetVisible(true);
		auto right = m_pOwner.lock()->GetMeshComponent()->GetChildByName(L"RightSword");
		right->SetVisible(false);

		//
		m_pOwner.lock()->GetMeshComponent()->GetChildByName(L"Slash")->SetVisible(true);

		currentPhase = OnThird;
		m_bOnCombo = false;

		SOUND->GetPtr(ESoundType::Slash_3)->PlayEffect2D();
	}
	break;
	case AttackCombo::OnThird:
	{
		End();
	}
	break;
	case AttackCombo::Done:
	{
		End();
	}
	break;
	default:
		break;
	}

}
void PlayerAttackState::End()
{
	// 기본 state 세팅
	m_bOnPlaying = false;
	auto animInstance = m_pOwner.lock()->GetMeshComponent<USkinnedMeshComponent>()->GetAnimInstance();
	animInstance->m_fAnimPlayRate = 25.0f;

	currentPhase = AttackCombo::OnFirst;
	reverse = false;

	// Sword
	auto left = m_pOwner.lock()->GetMeshComponent()->GetChildByName(L"LeftSword");
	left->SetVisible(false);
	auto right = m_pOwner.lock()->GetMeshComponent()->GetChildByName(L"RightSword");
	right->SetVisible(false);
	auto back = m_pOwner.lock()->GetMeshComponent()->GetChildByName(L"BackSword");
	back->SetVisible(true);

	//
	m_bCanBeHit = false;

	// 
	m_pAttackRange.lock()->m_bCollision = false;

	// 
	m_pOwner.lock()->GetMeshComponent()->GetChildByName(L"Slash")->SetVisible(false);
}

void PlayerAttackState::CheckMouse()
{
	MouseRay ray;
	ray.Click();

	auto playerPos = m_pOwner.lock()->GetPosition();
	playerPos.y += 1.f;
	Vec3 inter;

	Collision::GetIntersection(ray, playerPos, Vec3(0.f, 1.f, 0.f), inter);
	dir = inter - m_pOwner.lock()->GetPosition();
}

void PlayerAttackState::Rotate()
{
	float targetYaw = atan2f(dir.x, dir.z);
	Vec3 currentRot = m_pOwner.lock()->GetRotation();
	currentRot.y = targetYaw;
	m_pOwner.lock()->SetRotation(currentRot);
}

void PlayerAttackState::Move()
{
	m_pOwner.lock()->SetMove(dir, 0.5f);
}

void PlayerAttackState::Slash()
{
	if (m_bSlashPlaying)
	{
		m_fSlashTime += TIMER->GetDeltaTime();

		float t = m_fSlashTime;
		float progress = 0.0f;


		if (t <= 0.3f)
		{
			float ratio = t / 0.3f;
			progress = pow(ratio, 2.0f);
		}
		else
		{
			progress = -1.0f;
		}

		if (m_pSlashMaterial)
			m_pSlashMaterial->SetSlashProgress(progress, reverse);

		if (t >= m_fSlashDuration)
		{
			m_bSlashPlaying = false;
			m_pOwner.lock()->GetMeshComponent()->GetChildByName(L"Slash")->SetVisible(false);

		}
	}
}

PlayerHitState::PlayerHitState(weak_ptr<AActor> _pOwner) : PlayerBaseState(PLAYER_S_HIT)
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
	SOUND->GetPtr(ESoundType::Hit)->PlayEffect2D();

	hitPos = m_pOwner.lock()->GetPosition();
}
void PlayerHitState::Tick()
{
	// 밀려나지 않게 고정시켜버림
	m_pOwner.lock()->SetPosition(hitPos);
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

	//m_pOwner.lock()->GetShapeComponent()->SetCollisionEnabled(CollisionEnabled::CE_QUERYANDPHYSICS);
}

PlayerDieState::PlayerDieState(weak_ptr<AActor> _pOwner) : PlayerBaseState(PLAYER_S_DEATH)
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
	SOUND->GetPtr(ESoundType::Die)->PlayEffect2D();
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

PlayerRollState::PlayerRollState(weak_ptr<AActor> _pOwner) : PlayerBaseState(PLAYER_S_ROLL)
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
	SOUND->GetPtr(ESoundType::Dash)->PlayEffect2D();

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
	m_bCanRoll = false;
}

PlayerShootState::PlayerShootState(weak_ptr<AActor> _pOwner) : PlayerBaseState(PLAYER_S_SHOOT)
{
	m_pOwner = _pOwner;
	m_bCanInterrupt = true;

	// sub-state
	start = make_shared<PlayerShootStart>(m_pOwner);
	shoot = make_shared<PlayerShoot>(m_pOwner);
}
void PlayerShootState::Enter()
{
	// 기본 state 세팅
	m_bOnPlaying = true;

	start->Enter();
	shoot->CheckShootCount(bCanShoot);
	m_pBow.lock()->m_bRender = true;
	UpdateBow();

	SOUND->GetPtr(ESoundType::Bow_Stretch)->PlayEffect2D();
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

			SOUND->GetPtr(ESoundType::Bow_Release)->PlayEffect2D();
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
	// 기본 state 세팅
	m_bOnPlaying = false;

	// reset
	currentPhase = ShootPhase::Start;
	auto animInstance = m_pOwner.lock()->GetMeshComponent<USkinnedMeshComponent>()->GetAnimInstance();
	animInstance->m_bPlay = true;
	animInstance->m_bOnPlayOnce = false;
	bEnd = false;

	// bow 세팅
	m_pBow.lock()->m_bRender = false;

}
void PlayerShootState::Rotate()
{
	//dir = m_pTarget.lock()->GetPosition() - m_pOwner.lock()->GetPosition();
	float targetYaw = atan2f(dir.x, dir.z);
	Vec3 currentRot = m_pOwner.lock()->GetRotation();
	float currentYaw = currentRot.y;

	// 각도 차이 계산
	float angleDiff = targetYaw - currentYaw;
	while (angleDiff > DD_PI)  angleDiff -= DD_PI * 2;
	while (angleDiff < -DD_PI) angleDiff += DD_PI * 2;

	// Lerp 계산
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

void PlayerShootState::UpdateBow()
{
	Vec3 bowPos = m_pOwner.lock()->GetMeshComponent()->GetChildByName(L"LeftHandSocket")->GetWorldPosition();
	m_pBow.lock()->SetPosition(bowPos);
	float targetYaw = atan2f(m_pOwner.lock()->GetLook().x, m_pOwner.lock()->GetLook().z);
	Vec3 currentRot = m_pOwner.lock()->GetRotation();
	currentRot.y = targetYaw;
	m_pBow.lock()->SetRotation(currentRot);
}

PlayerShootStart::PlayerShootStart(weak_ptr<AActor> _pOwner) : PlayerBaseState(PLAYER_S_SHOOT)
{
	m_pOwner = _pOwner;
	m_bCanInterrupt = true;
}
void PlayerShootStart::Enter()
{
	// 기본 state 세팅
	m_bOnPlaying = true;

	// 애니메이션 Attack 플레이
	auto animInstance = m_pOwner.lock()->GetMeshComponent<USkinnedMeshComponent>()->GetAnimInstance();
	int index = animInstance->GetAnimIndex(L"Arrow");
	animInstance->PlayOnce(index);
	// pr timer로 조절 ( 시간 맞추기 어려우면 anim key event 추가로 바꾸자 ) 
	// 애니메이션 속도가 달라지면 이거 안돼 -> 무조건 프레임 비교로 해야돼


}
void PlayerShootStart::Tick()
{
	auto animInstance = m_pOwner.lock()->GetMeshComponent<USkinnedMeshComponent>()->GetAnimInstance();
	if (animInstance->GetCurrentFrame() >= 20)
	{
		animInstance->m_bPlay = false;
		End();
	}
}
void PlayerShootStart::End()
{
	// 기본 state 세팅
	m_bOnPlaying = false;
}

PlayerShoot::PlayerShoot(weak_ptr<AActor> _pOwner) : PlayerBaseState(PLAYER_S_SHOOT)
{
	m_pOwner = _pOwner;
	m_bCanInterrupt = true;

	// AnimSetting
	auto animInstance = m_pOwner.lock()->GetMeshComponent<USkinnedMeshComponent>()->GetAnimInstance();
	int index = animInstance->GetAnimIndex(L"Arrow");
	animInstance->AddEvent(index, 22, [this]() {
		this->CanShoot();
		});
}
void PlayerShoot::Enter()
{
	// 기본 state 세팅
	m_bOnPlaying = true;

	// 애니메이션 Attack 플레이
	auto animInstance = m_pOwner.lock()->GetMeshComponent<USkinnedMeshComponent>()->GetAnimInstance();
	animInstance->m_bPlay = true;
}
void PlayerShoot::Tick()
{
	auto animInstance = m_pOwner.lock()->GetMeshComponent<USkinnedMeshComponent>()->GetAnimInstance();
	if (!animInstance->m_bOnPlayOnce)
	{
		// 애니메이션 종료
		End();
	}
}
void PlayerShoot::End()
{
	// 기본 state 세팅
	m_bOnPlaying = false;
}
void PlayerShoot::CheckShootCount(bool _able)
{
	bCanShoot = _able;
}
void PlayerShoot::CanShoot()
{
	if (!bCanShoot) { return; }
	// 투사체 발사
	auto  hand = m_pOwner.lock()->GetMeshComponent()->GetChildByName(L"LeftHandSocket");
	Vec3 pos = hand->GetWorldPosition();

	Vec3 look = m_pOwner.lock()->GetLook();
	PROJECTILE->ActivateOne(ProjectileType::PlayerArrow, pos, look);

	dynamic_pointer_cast<TPlayer>(m_pOwner.lock())->DecArrowCount();
}

PlayerClimbState::PlayerClimbState(weak_ptr<AActor> _pOwner) : PlayerBaseState(PLAYER_S_CLIMB)
{
	m_pOwner = _pOwner;
	m_bCanInterrupt = false;
	finish = make_shared<PlayerClimbFinish>(m_pOwner);
}
void PlayerClimbState::Enter()
{
	// 기본 state 세팅
	m_bOnPlaying = true;

	// 애니메이션 Attack 플레이
	auto animInstance = m_pOwner.lock()->GetMeshComponent<USkinnedMeshComponent>()->GetAnimInstance();
	int attackIndex = animInstance->GetAnimIndex(L"Climbing_ladder");
	animInstance->SetCurrentAnimTrack(attackIndex);

	// 중력 적용
	m_pOwner.lock()->GetPhysicsComponent()->SetWeight(0.f);

	//if (isMoving)
	//{
	//	SOUND->GetPtr(ESoundType::Ladder)->PlayEffect2D();
	//}
	//else
	//{
	//	SOUND->GetPtr(ESoundType::Ladder)->Paused();
	//}
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
	// 기본 state 세팅
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

PlayerClimbFinish::PlayerClimbFinish(weak_ptr<AActor> _pOwner) : PlayerBaseState(PLAYER_S_CLIMB)
{
	m_pOwner = _pOwner;
	m_bCanInterrupt = false;
}
void PlayerClimbFinish::Enter()
{
	// 기본 state 세팅
	m_bOnPlaying = true;

	// 애니메이션 Attack 플레이
	auto animInstance = m_pOwner.lock()->GetMeshComponent<USkinnedMeshComponent>()->GetAnimInstance();
	int index = animInstance->GetAnimIndex(L"Climbing_off_ladder_top");
	animInstance->m_bPlay = true;
	animInstance->PlayOnce(index);
}
void PlayerClimbFinish::Tick()
{
	// AddPosition 조금
	ladderDir.Normalize();
	Vec3 look = ladderDir;
	Vec3 up = m_pOwner.lock()->GetUp();

	Vec3 moveDir = look * 1.0f + up * 0.5f;

	moveDir.Normalize();
	moveDir = moveDir * offset * TIMER->GetDeltaTime();

	// 값 
	m_pOwner.lock()->AddPosition(moveDir);

	auto animInstance = m_pOwner.lock()->GetMeshComponent<USkinnedMeshComponent>()->GetAnimInstance();
	if (!animInstance->m_bOnPlayOnce)
	{
		// 애니메이션 종료
		m_pOwner.lock()->GetPhysicsComponent()->SetWeight(1.f);
		dynamic_pointer_cast<TPlayer>(m_pOwner.lock())->StopClimbing();
		End();
	}
}
void PlayerClimbFinish::End()
{
	// 기본 state 세팅
	m_bOnPlaying = false;
}

void PlayerClimbFinish::SetLadderDir(Vec3 _dir)
{
	ladderDir = _dir;
}

PlayerBaseState::PlayerBaseState(UINT _iStateId) : StateBase(_iStateId)
{
	
}
