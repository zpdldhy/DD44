#include "pch.h"
#include "BettyStates.h"

#include "AActor.h"
#include "USkinnedMeshComponent.h"
#include "UAnimInstance.h"

#include "Sound.h"
#include "Timer.h"

#include "UBoxComponent.h"
#include "CollisionManager.h"
#include "ObjectManager.h"
#include "EffectManager.h"

void JumpMotionHandler::Init(Vec3 _start, Vec3 _end, float _height, float _total, float _hangDuration, float _fall)
{
	// 초기화
	bDone = false;
	phase = JumpPhase::Ascending;

	start = _start;
	end = _end;
	height = _height;
	totalTime = _total;
	elapsed = 0.0f;
	hangDuration = _hangDuration;
	fall = _fall;

	// 계산
	Vec3 distance = end - start;
	velocity.x = distance.x / totalTime;
	velocity.z = distance.z / totalTime;
	gravity = (8.0f * height) / (totalTime * totalTime);
	velocity.y = gravity * (totalTime / 2.0f);
}
Vec3 JumpMotionHandler::Update()
{
	Vec3 pos;
	elapsed += TIMER->GetDeltaTime();
	switch (phase)
	{
	case JumpPhase::Ascending:
		pos.x = start.x + velocity.x * elapsed;
		pos.z = start.z + velocity.z * elapsed;
		pos.y = start.y + velocity.y * elapsed - 0.5f * gravity * elapsed * elapsed;

		if (elapsed >= totalTime * 0.5f)
		{
			// 최고점 위치 고정
			pos.y = start.y + height;
			hangElapsed = 0.0f;
			phase = JumpPhase::Hanging;
		}
		break;

	case JumpPhase::Hanging:
		hangElapsed += TIMER->GetDeltaTime();
		pos.x = start.x + velocity.x * elapsed;
		pos.z = start.z + velocity.z * elapsed;
		pos.y = start.y + height;

		if (hangElapsed >= hangDuration)
		{
			phase = JumpPhase::Falling;
			prevPos = pos;
			elapsed = 0.0f; // 낙하 시간 초기화
		}
		break;

	case JumpPhase::Falling:
	{
		elapsed += TIMER->GetDeltaTime();
		float fallGravity = gravity * fall; // 가속

		pos.x = prevPos.x + (end.x - prevPos.x) * elapsed;
		pos.z = prevPos.z + (end.z - prevPos.z) * elapsed;
		pos.y = start.y + height - 0.5f * fallGravity * elapsed * elapsed;

		if (pos.y <= start.y)
		{
			pos.y = start.y;
			phase = JumpPhase::Done;
		}
		break;

	}
	case JumpPhase::Done:
		bDone = true;
		break;
	}
	return pos;
}

BettyIdleState::BettyIdleState(weak_ptr<AActor> _pOwner) : StateBase(BETTY_S_IDLE)
{
	m_pOwner = _pOwner;
	m_bCanInterrupt = true;
}
void BettyIdleState::Enter()
{
	// 기본 state 세팅
	m_bOnPlaying = true;
	// 애니메이션 Idle 플레이
	auto animInstance = m_pOwner.lock()->GetMeshComponent<USkinnedMeshComponent>()->GetAnimInstance();
	int idleIndex = animInstance->GetAnimIndex(L"Armature|Idle_Beast");
	animInstance->SetCurrentAnimTrack(idleIndex);
}
void BettyIdleState::Tick()
{

}
void BettyIdleState::End()
{
	// 기본 state 세팅
	m_bOnPlaying = false;
}

BettyIntroState::BettyIntroState(weak_ptr<AActor> _pOwner) : StateBase(BETTY_S_INTRO)
{
	m_pOwner = _pOwner;
	m_bCanInterrupt = false;
}
void BettyIntroState::Enter()
{
	// 기본 state 세팅
	m_bOnPlaying = true;
	// 애니메이션 Idle 플레이
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
			animInstance->AddEvent(index, 24, [this]() {
				this->PlayFx();
				});
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
	// 기본 state 세팅
	m_bOnPlaying = false;
}
void BettyIntroState::PlayFx()
{
	auto left = m_pOwner.lock()->GetMeshComponent()->GetChildByName(L"LeftHand");
	auto right = m_pOwner.lock()->GetMeshComponent()->GetChildByName(L"RightHand");

	Vec3 temp = left->GetWorldPosition() + right->GetWorldPosition();
	Vec3 pos = (temp) / 2.0f;
	pos.y = m_pOwner.lock()->GetPosition().y;
	EFFECT->PlayDustBurst(pos, 10.f, .8f);
	EFFECT->PlayEffect(EEffectType::Shockwave, pos, 0.f, Vec3::Zero, .8f);
	EFFECT->PlayBeamBurst(pos, 20, .7f);
}

BettyJumpAttack::BettyJumpAttack(weak_ptr<AActor> _pOwner) : StateBase(BETTY_S_A_JUMPHIGH)
{
	m_pOwner = _pOwner;
	m_bCanInterrupt = false;
}
void BettyJumpAttack::Enter()
{
	// 기본 state 세팅
	m_bOnPlaying = true;
	// 애니메이션
	auto animInstance = m_pOwner.lock()->GetMeshComponent<USkinnedMeshComponent>()->GetAnimInstance();
	int index = animInstance->GetAnimIndex(L"Armature|Punch_double");
	animInstance->PlayOnce(index);

	animInstance->AddEvent(index, 25, [this]() {
		this->SlowAnimSpeed();
		});
	animInstance->AddEvent(index, 26, [this]() {
		this->PlayBodyFx();
		});
	animInstance->AddEvent(index, 35, [this]() {
		this->NormalizeAnimSpeed();
		});
	animInstance->AddEvent(index, 31, [this]() {
		this->PlayHandFx();
		});

	//// 사운드
	//SOUNDMANAGER->GetPtr(ESoundType::Attack_Bat)->PlayEffect2D();

	// 이동
	auto look = m_pOwner.lock()->GetLook();
	look.y = 0;
	look.Normalize();
	Vec3 startPos = m_pOwner.lock()->GetPosition();
	Vec3 endPos = targetPos;
	//Vec3 endPos = startPos + posOffset * look;
	float totalTime = (animInstance->GetTotalFrame()) / TIMER->GetFPS();
	jumpHandler.Init(startPos, endPos, 8.0f, totalTime, 0.1f, 2.0f);

	// 회전
	Vec3 dir = endPos - startPos;
	targetYaw = atan2f(dir.x, dir.z);
}
void BettyJumpAttack::Tick()
{
	// Anim
	auto animInstance = m_pOwner.lock()->GetMeshComponent<USkinnedMeshComponent>()->GetAnimInstance();
	if (!animInstance->m_bOnPlayOnce && jumpHandler.IsDone())
	{
		End();
	}
	// 이동
	Vec3 pos = jumpHandler.Update();
	if (!jumpHandler.IsDone())
	{
		m_pOwner.lock()->SetPosition(pos);
	}
	// Lerp 회전
	{
		Vec3 currentRot = m_pOwner.lock()->GetRotation();
		float currentYaw = currentRot.y;
		float angleDiff = targetYaw - currentYaw;
		while (angleDiff > DD_PI)  angleDiff -= DD_PI * 2;
		while (angleDiff < -DD_PI) angleDiff += DD_PI * 2;
		float smoothedYaw = currentRot.y + angleDiff * 8.0f * TIMER->GetDeltaTime();
		currentRot.y = smoothedYaw;
		m_pOwner.lock()->SetRotation(currentRot);
	}
}
void BettyJumpAttack::End()
{
	// 기본 state 세팅
	//phase = JumpPhase::Ascending;
	m_bOnPlaying = false;

	bControlOnce = true;
}
void BettyJumpAttack::SlowAnimSpeed()
{
	if (bControlOnce)
	{
		auto animInstance = m_pOwner.lock()->GetMeshComponent<USkinnedMeshComponent>()->GetAnimInstance();
		originAnimSpeed = animInstance->m_fAnimPlayRate;
		animInstance->m_fAnimPlayRate -= 5.0f;
		bControlOnce = false;
	}
}
void BettyJumpAttack::NormalizeAnimSpeed()
{
	auto animInstance = m_pOwner.lock()->GetMeshComponent<USkinnedMeshComponent>()->GetAnimInstance();
	animInstance->m_fAnimPlayRate = originAnimSpeed;
}
void BettyJumpAttack::PlayHandFx()
{
	// 사운드 ( 소스 변경 필요 ) 
	SOUNDMANAGER->GetPtr(ESoundType::Attack_Bat)->PlayEffect2D();

	auto left = m_pOwner.lock()->GetMeshComponent()->GetChildByName(L"LeftHand");
	auto right = m_pOwner.lock()->GetMeshComponent()->GetChildByName(L"RightHand");

	Vec3 temp = left->GetWorldPosition() + right->GetWorldPosition();
	Vec3 pos = (temp) / 2.0f;
	pos.y = m_pOwner.lock()->GetPosition().y;
	EFFECT->PlayDustBurst(pos, 10.f, .5f);
	EFFECT->PlayEffect(EEffectType::Shockwave, pos, 0.f, Vec3::Zero, .5f);
	EFFECT->PlayBeamBurst(pos, 20, .5f);
}
void BettyJumpAttack::PlayBodyFx()
{
	// 사운드 ( 소스 변경 필요 )
	SOUNDMANAGER->GetPtr(ESoundType::Attack_Bat)->PlayEffect2D();

	Vec3 pos = m_pOwner.lock()->GetPosition();
	EFFECT->PlayDustBurst(pos, 10.f, .8f);
	EFFECT->PlayEffect(EEffectType::Shockwave, pos, 0.f, Vec3::Zero, .5f);
	//EFFECT->PlayBeamBurst(pos, 20, .5f);
}

BettyTwoHandAttack::BettyTwoHandAttack(weak_ptr<AActor> _pOwner) : StateBase(BETTY_S_A_TWOHAND)
{
	m_pOwner = _pOwner;
	m_bCanInterrupt = false;
}
void BettyTwoHandAttack::Enter()
{
	// 기본 state 세팅
	m_bOnPlaying = true;
	// 애니메이션 Idle 플레이
	auto animInstance = m_pOwner.lock()->GetMeshComponent<USkinnedMeshComponent>()->GetAnimInstance();
	int index = animInstance->GetAnimIndex(L"Armature|FistSlam");
	animInstance->PlayOnce(index);
}
void BettyTwoHandAttack::Tick()
{
	// Anim
	auto animInstance = m_pOwner.lock()->GetMeshComponent<USkinnedMeshComponent>()->GetAnimInstance();
	if (!animInstance->m_bOnPlayOnce)
	{
		End();
	}
}
void BettyTwoHandAttack::End()
{
	// 기본 state 세팅
	m_bOnPlaying = false;
}

BettyOneHandBackAttack::BettyOneHandBackAttack(weak_ptr<AActor> _pOwner) : StateBase(BETTY_S_A_HANDBACK)
{
	m_pOwner = _pOwner;
	m_bCanInterrupt = false;
}
void BettyOneHandBackAttack::Enter()
{
	// 기본 state 세팅
	m_bOnPlaying = true;
	// 애니메이션 Idle 플레이
	auto animInstance = m_pOwner.lock()->GetMeshComponent<USkinnedMeshComponent>()->GetAnimInstance();
	int index = animInstance->GetAnimIndex(L"Armature|Slash_right");
	if (!bRight)
	{
		index = animInstance->GetAnimIndex(L"Armature|Slash_left");
	}
	animInstance->PlayOnce(index);
}
void BettyOneHandBackAttack::Tick()
{
	// Anim
	auto animInstance = m_pOwner.lock()->GetMeshComponent<USkinnedMeshComponent>()->GetAnimInstance();
	if (!animInstance->m_bOnPlayOnce)
	{
		End();
	}
	// Lerp 회전
	{
		Vec3 currentRot = m_pOwner.lock()->GetRotation();
		float currentYaw = currentRot.y;
		float angleDiff = targetYaw - currentYaw;
		while (angleDiff > DD_PI)  angleDiff -= DD_PI * 2;
		while (angleDiff < -DD_PI) angleDiff += DD_PI * 2;
		float smoothedYaw = currentRot.y + angleDiff * 10.0f * TIMER->GetDeltaTime();
		currentRot.y = smoothedYaw;
		m_pOwner.lock()->SetRotation(currentRot);
	}
}
void BettyOneHandBackAttack::End()
{
	// 기본 state 세팅
	m_bOnPlaying = false;
}
bool BettyOneHandBackAttack::CheckDirection(Vec3 _targetPos)
{
	Vec3 look = m_pOwner.lock()->GetLook();
	look.Normalize();
	Vec3 currentPos = m_pOwner.lock()->GetPosition();
	Vec3 dir = _targetPos - currentPos;

	Vec3 cross = look.Cross(dir);

	if (cross.y > 0.0f)
	{
		bRight = true;
	}
	//else if (cross.y < 0.0f)
	else
	{
		bRight = false;
	}
	targetYaw = atan2f(dir.x, dir.z);

	return bRight;
}

BettyOneHandDownAttack::BettyOneHandDownAttack(weak_ptr<AActor> _pOwner) : StateBase(BETTY_S_A_HANDDOWN)
{
	m_pOwner = _pOwner;
	m_bCanInterrupt = false;
}
void BettyOneHandDownAttack::Enter()
{
	// 기본 state 세팅
	m_bOnPlaying = true;
	// 애니메이션 Idle 플레이
	auto animInstance = m_pOwner.lock()->GetMeshComponent<USkinnedMeshComponent>()->GetAnimInstance();
	int index = animInstance->GetAnimIndex(L"Armature|Punch_right");
	if (!bRight)
	{
		index = animInstance->GetAnimIndex(L"Armature|Punch_left");
	}
	animInstance->PlayOnce(index);
}
void BettyOneHandDownAttack::Tick()
{
	// Anim
	auto animInstance = m_pOwner.lock()->GetMeshComponent<USkinnedMeshComponent>()->GetAnimInstance();
	if (!animInstance->m_bOnPlayOnce)
	{
		End();
	}
	// Lerp 회전
	{
		Vec3 currentRot = m_pOwner.lock()->GetRotation();
		float currentYaw = currentRot.y;
		float angleDiff = targetYaw - currentYaw;
		while (angleDiff > DD_PI)  angleDiff -= DD_PI * 2;
		while (angleDiff < -DD_PI) angleDiff += DD_PI * 2;
		float smoothedYaw = currentRot.y + angleDiff * 10.0f * TIMER->GetDeltaTime();
		currentRot.y = smoothedYaw;
		m_pOwner.lock()->SetRotation(currentRot);
	}
}
void BettyOneHandDownAttack::End()
{
	// 기본 state 세팅
	m_bOnPlaying = false;
}
bool BettyOneHandDownAttack::CheckDirection(Vec3 _targetPos)
{
	Vec3 look = m_pOwner.lock()->GetLook();
	look.Normalize();
	Vec3 currentPos = m_pOwner.lock()->GetPosition();
	Vec3 dir = _targetPos - currentPos;

	Vec3 cross = look.Cross(dir);

	if (cross.y > 0.0f)
	{
		bRight = true;
	}
	else
	{
		bRight = false;
	}
	targetYaw = atan2f(dir.x, dir.z);

	return bRight;
}

BettyRollAttack::BettyRollAttack(weak_ptr<AActor> _pOwner) : StateBase(BETTY_S_A_ROLL)
{
	m_pOwner = _pOwner;
	m_bCanInterrupt = false;

	start = make_shared<BettyRollStart>(m_pOwner.lock());
	middle = make_shared<BettyRollMiddle>(m_pOwner.lock());
	end = make_shared<BettyRollEnd>(m_pOwner.lock());

}
void BettyRollAttack::Enter()
{
	// 기본 init
	m_bOnPlaying = true;

	start->SetDirection(dir);
	//middle->SetDirection(dir);
	end->SetDirection(dir);
	start->Enter();
	rollCount--;

}
void BettyRollAttack::Tick()
{
	switch (currentPhase)
	{
	case RollPhase::Start:
		start->Tick();
		if (!start->IsPlaying())
		{
			dir = m_pTarget.lock()->GetPosition() - m_pOwner.lock()->GetPosition();
			middle->SetDirection(dir);
			middle->Enter();
			currentPhase = RollPhase::Middle;
		}
		break;
	case RollPhase::Middle:
		middle->Tick();
		rollElapsed += TIMER->GetDeltaTime();
		//if (rollElapsed > 0.5f)
		//{
		//	rollElapsed = 0.0f;
		//	bHitWall = true;
		//}
		bHitWall = middle->CheckWallCollision();
		if (bHitWall)
		{
			// FX
			PlayFx();
			middle->End(); // 얘는 end 꼭 여기서.
			end->Enter();
			currentPhase = RollPhase::Final;
		}
		break;
	case RollPhase::Final:
		end->Tick();
		if (!end->IsPlaying())
		{
			if (rollCount > 0)
			{
				currentPhase = RollPhase::Start;
				bHitWall = false;
				dir = m_pTarget.lock()->GetPosition() - m_pOwner.lock()->GetPosition();
				Enter();
			}
			else
			{
				currentPhase = RollPhase::Done;
			}

		}
		break;
	case RollPhase::Done:
		End();
		break;
	}
}
void BettyRollAttack::End()
{
	// 기본 state 세팅
	m_bOnPlaying = false;
	currentPhase = RollPhase::Start;
	bHitWall = false;

	rollCount = 3;
	rollElapsed = 0.0f;

	//m_pTarget.reset();
}
void BettyRollAttack::SetTarget(weak_ptr<AActor> _target)
{
	m_pTarget = _target;

	dir = m_pTarget.lock()->GetPosition() - m_pOwner.lock()->GetPosition();
	dir.y = 0;
	dir.Normalize();
}
void BettyRollAttack::PlayFx()
{
	// 사운드 ( 소스 변경 필요 )
	SOUNDMANAGER->GetPtr(ESoundType::Attack_Bat)->PlayEffect2D();

	Vec3 pos = m_pOwner.lock()->GetPosition();
	EFFECT->PlayDustBurst(pos, 10.f, .8f);
	EFFECT->PlayEffect(EEffectType::Shockwave, pos, 0.f, Vec3::Zero, .5f);
	//EFFECT->PlayBeamBurst(pos, 20, .5f);
}
#pragma region roll_subState
BettyRollStart::BettyRollStart(weak_ptr<AActor> _pOwner) : StateBase(BETTY_S_A_ROLL)
{
	m_pOwner = _pOwner;
	m_bCanInterrupt = false;
}
void BettyRollStart::Enter()
{
	// 기본 state 세팅
	m_bOnPlaying = true;
	// 애니메이션 Idle 플레이
	auto animInstance = m_pOwner.lock()->GetMeshComponent<USkinnedMeshComponent>()->GetAnimInstance();
	int index = animInstance->GetAnimIndex(L"Armature|JumpSpin");
	// spin과 바로 연결되도록
	animInstance->SetKeyFrame(index, 17);
	animInstance->PlayOnce(index);
	// 점프 모션
	auto look = m_pOwner.lock()->GetLook();
	look.y = 0;
	look.Normalize();
	Vec3 startPos = m_pOwner.lock()->GetPosition();
	Vec3 endPos = startPos + posOffset * look;
	float totalTime = animInstance->GetTotalFrame() / TIMER->GetFPS();
	jumpHandler.Init(startPos, endPos, 8.0f, totalTime, 0.1f, 3.0f);

	// 회전
	targetYaw = atan2f(dir.x, dir.z);
}
void BettyRollStart::Tick()
{
	// Anim
	auto animInstance = m_pOwner.lock()->GetMeshComponent<USkinnedMeshComponent>()->GetAnimInstance();
	if (!animInstance->m_bOnPlayOnce && jumpHandler.IsDone())
	{
		End();
	}

	// Jump 
	Vec3 pos = jumpHandler.Update();
	if (!jumpHandler.IsDone())
	{
		m_pOwner.lock()->SetPosition(pos);
	}

	// Lerp 회전
	{
		Vec3 currentRot = m_pOwner.lock()->GetRotation();
		float currentYaw = currentRot.y;
		float angleDiff = targetYaw - currentYaw;
		while (angleDiff > DD_PI)  angleDiff -= DD_PI * 2;
		while (angleDiff < -DD_PI) angleDiff += DD_PI * 2;
		float smoothedYaw = currentRot.y + angleDiff * 8.0f * TIMER->GetDeltaTime();
		currentRot.y = smoothedYaw;
		m_pOwner.lock()->SetRotation(currentRot);

	}
}
void BettyRollStart::End()
{
	// 기본 state 세팅
	m_bOnPlaying = false;
}

BettyRollMiddle::BettyRollMiddle(weak_ptr<AActor> _pOwner) : StateBase(BETTY_S_A_ROLL)
{
	m_pOwner = _pOwner;
	m_bCanInterrupt = false;
}
void BettyRollMiddle::Enter()
{
	// 기본 state 세팅
	m_bOnPlaying = true;
	// 애니메이션 Idle 플레이
	auto animInstance = m_pOwner.lock()->GetMeshComponent<USkinnedMeshComponent>()->GetAnimInstance();
	int index = animInstance->GetAnimIndex(L"Armature|Spin");
	animInstance->SetCurrentAnimTrack(index);
	// 회전
	float targetYaw = atan2f(dir.x, dir.z);
	Vec3 currentRot = m_pOwner.lock()->GetRotation();
	currentRot.y = targetYaw;
	m_pOwner.lock()->SetRotation(currentRot);
}
void BettyRollMiddle::Tick()
{
	// 충돌 확인 후 End ( 밖에서 ) 
	dir.y = 0;
	dir.Normalize();
	m_pOwner.lock()->AddPosition(dir * 1.0f);
	CheckWallCollision();
}
void BettyRollMiddle::End()
{
	// 기본 state 세팅
	m_bOnPlaying = false;
}

bool BettyRollMiddle::CheckWallCollision()
{
	Vec3 pos = m_pOwner.lock()->GetPosition();
	Vec3 look = dir;

	float offset = 4.0f;
	Vec3 right = m_pOwner.lock()->GetRight();
	Ray leftRay(pos + (-right * offset), look);
	Ray rightRay(pos + (right * offset), look);
	Ray middleRay(pos, look);

	auto box = dynamic_pointer_cast<UBoxComponent>(m_pOwner.lock()->GetShapeComponent());
	auto ray = box->GetLookRay();
	Vec3 range(5.0f, 0.0f, 5.0f);

	for (const auto& colData : m_pOwner.lock()->m_vCollisionList)
	{
		auto targetShape = OBJECT->GetActor(colData.first)->GetShapeComponent();
		auto targetBox = dynamic_pointer_cast<UBoxComponent>(targetShape);

		Vec3 inter; 
		if (Collision::CheckRayToOBB(middleRay, targetBox->GetBounds(), inter))
		{
			// 
			Vec3 dis = pos - inter;
			dis.y = 0;
			if (dis.Length() < range.Length())
			{
				return true;
			}
		}

		if (Collision::CheckRayToOBB(leftRay, targetBox->GetBounds(), inter))
		{
			// 
			Vec3 dis = pos - inter;
			dis.y = 0;
			if (dis.Length() < range.Length())
			{
				return true;
			}
		}

		if (Collision::CheckRayToOBB(rightRay, targetBox->GetBounds(), inter))
		{
			// 
			Vec3 dis = pos - inter;
			dis.y = 0;
			if (dis.Length() < range.Length())
			{
				return true;
			}
		}
	}
	return false;
}

BettyRollEnd::BettyRollEnd(weak_ptr<AActor> _pOwner) : StateBase(BETTY_S_A_ROLL)
{
	m_pOwner = _pOwner;
	m_bCanInterrupt = false;
}
void BettyRollEnd::Enter()
{
	// 기본 state 세팅
	m_bOnPlaying = true;
	// 애니메이션 Idle 플레이
	auto animInstance = m_pOwner.lock()->GetMeshComponent<USkinnedMeshComponent>()->GetAnimInstance();
	int index = animInstance->GetAnimIndex(L"Armature|Spin_Collide");
	animInstance->PlayOnce(index);
	// 점프 모션
	auto look = m_pOwner.lock()->GetLook();
	look.y = 0;
	look = -look;
	look.Normalize();
	Vec3 startPos = m_pOwner.lock()->GetPosition();
	// 왜 ? 모르겟음. 나중에 변경 필요 .. 
	startPos.y = 0;
	Vec3 endPos = startPos + posOffset * look;
	float totalTime = animInstance->GetTotalFrame() / TIMER->GetFPS();
	jumpHandler.Init(startPos, endPos, 8.0f, totalTime, 0.1f, 3.0f);

	// 회전
	dir = -dir;
	targetYaw = atan2f(look.x, look.z);
}
void BettyRollEnd::Tick()
{
	// Anim
	auto animInstance = m_pOwner.lock()->GetMeshComponent<USkinnedMeshComponent>()->GetAnimInstance();
	if (!animInstance->m_bOnPlayOnce)
	{
		End();
	}
	// Jump 
	Vec3 pos = jumpHandler.Update();
	if (!jumpHandler.IsDone())
	{
		m_pOwner.lock()->SetPosition(pos);
	}
	// Lerp 회전
	{
		Vec3 currentRot = m_pOwner.lock()->GetRotation();
		float currentYaw = currentRot.y;
		float angleDiff = targetYaw - currentYaw;
		while (angleDiff > DD_PI)  angleDiff -= DD_PI * 2;
		while (angleDiff < -DD_PI) angleDiff += DD_PI * 2;
		float smoothedYaw = currentRot.y + angleDiff * 8.0f * TIMER->GetDeltaTime();
		currentRot.y = smoothedYaw;
		m_pOwner.lock()->SetRotation(currentRot);

	}
}
void BettyRollEnd::End()
{
	// 기본 state 세팅
	m_bOnPlaying = false;
}
#pragma endregion

BettyDropAttack::BettyDropAttack(weak_ptr<AActor> _pOwner) : StateBase(BETTY_S_A_DROPSNOW)
{
	m_pOwner = _pOwner;
	m_bCanInterrupt = false;
}
void BettyDropAttack::Enter()
{
	// 기본 state 세팅
	m_bOnPlaying = true;
	// 애니메이션 Idle 플레이
	auto animInstance = m_pOwner.lock()->GetMeshComponent<USkinnedMeshComponent>()->GetAnimInstance();
	int index = animInstance->GetAnimIndex(L"Armature|FistSlam_anger");
	animInstance->PlayOnce(index);
}
void BettyDropAttack::Tick()
{
	// Anim
	auto animInstance = m_pOwner.lock()->GetMeshComponent<USkinnedMeshComponent>()->GetAnimInstance();
	if (!animInstance->m_bOnPlayOnce)
	{
		End();
	}
}
void BettyDropAttack::End()
{
	// 기본 state 세팅
	m_bOnPlaying = false;
}

BettyRoarAttack::BettyRoarAttack(weak_ptr<AActor> _pOwner) : StateBase(BETTY_S_A_ROAR)
{
	m_pOwner = _pOwner;
	m_bCanInterrupt = false;
}
void BettyRoarAttack::Enter()
{
	// 기본 state 세팅
	m_bOnPlaying = true;
	// 애니메이션 Idle 플레이
	auto animInstance = m_pOwner.lock()->GetMeshComponent<USkinnedMeshComponent>()->GetAnimInstance();
	int index = animInstance->GetAnimIndex(L"Armature|Roar");
	animInstance->PlayOnce(index);
}
void BettyRoarAttack::Tick()
{
	// Anim
	auto animInstance = m_pOwner.lock()->GetMeshComponent<USkinnedMeshComponent>()->GetAnimInstance();
	if (!animInstance->m_bOnPlayOnce)
	{
		End();
	}
}
void BettyRoarAttack::End()
{
	// 기본 state 세팅
	m_bOnPlaying = false;
}

BettyDeathState::BettyDeathState(weak_ptr<AActor> _pOwner) : StateBase(BETTY_S_DEATH)
{
	m_pOwner = _pOwner;
	m_bCanInterrupt = false;
}
void BettyDeathState::Enter()
{
	// 기본 state 세팅
	m_bOnPlaying = true;
	// 애니메이션 Idle 플레이
	auto animInstance = m_pOwner.lock()->GetMeshComponent<USkinnedMeshComponent>()->GetAnimInstance();
	int index = animInstance->GetAnimIndex(L"Armature|Death_idle");
	animInstance->SetKeyFrame(index, 15);
	animInstance->PlayOnce(index);
}
void BettyDeathState::Tick()
{

}
void BettyDeathState::End()
{
}
