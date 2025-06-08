#include "pch.h"
#include "BettyStates.h"

#include "AActor.h"
#include "USkinnedMeshComponent.h"
#include "UAnimInstance.h"

#include "Sound.h"
#include "Timer.h"

void JumpMotionHandler::Init(Vec3 _start, Vec3 _end, float _height, float _total, float _hangDuration, float _fall)
{
	// �ʱ�ȭ
	bDone = false;
	phase = JumpPhase::Ascending;

	start = _start;
	end = _end;
	height = _height;
	totalTime = _total;
	elapsed = 0.0f;
	hangDuration = _hangDuration;
	fall = _fall;

	// ���
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
			// �ְ��� ��ġ ����
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
			elapsed = 0.0f; // ���� �ð� �ʱ�ȭ
		}
		break;

	case JumpPhase::Falling:
	{
		elapsed += TIMER->GetDeltaTime();
		float fallGravity = gravity * fall; // ����
		pos.x = end.x;
		pos.z = end.z;
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
	// �⺻ state ����
	m_bOnPlaying = true;
	// �ִϸ��̼� Idle �÷���
	auto animInstance = m_pOwner.lock()->GetMeshComponent<USkinnedMeshComponent>()->GetAnimInstance();
	int idleIndex = animInstance->GetAnimIndex(L"Armature|Idle_Beast");
	animInstance->SetCurrentAnimTrack(idleIndex);
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

BettyJumpAttack::BettyJumpAttack(weak_ptr<AActor> _pOwner) : StateBase(BETTY_S_A_JUMPHIGH)
{
	m_pOwner = _pOwner;
	m_bCanInterrupt = false;
}
void BettyJumpAttack::Enter()
{
	// �⺻ state ����
	m_bOnPlaying = true;
	// �ִϸ��̼�
	auto animInstance = m_pOwner.lock()->GetMeshComponent<USkinnedMeshComponent>()->GetAnimInstance();
	int index = animInstance->GetAnimIndex(L"Armature|Punch_double");
	animInstance->PlayOnce(index);
	// ����
	SOUNDMANAGER->GetPtr(ESoundType::Attack_Bat)->PlayEffect2D();

	// �̵�
	auto look = m_pOwner.lock()->GetLook();
	look.y = 0;
	look.Normalize();
	Vec3 startPos = m_pOwner.lock()->GetPosition();
	Vec3 endPos = targetPos;
	//Vec3 endPos = startPos + posOffset * look;
	float totalTime = animInstance->GetTotalFrame() / TIMER->GetFPS();
	jumpHandler.Init(startPos, endPos, 8.0f, totalTime, 0.4f);

	// ȸ��
	Vec3 dir = endPos - startPos;
	targetYaw = atan2f(dir.x, dir.z);
}
void BettyJumpAttack::Tick()
{
	// Anim
	auto animInstance = m_pOwner.lock()->GetMeshComponent<USkinnedMeshComponent>()->GetAnimInstance();
	if (!animInstance->m_bOnPlayOnce)
	{
		End();
	}
	// �̵�
	Vec3 pos = jumpHandler.Update();
	if (!jumpHandler.IsDone())
	{
		m_pOwner.lock()->SetPosition(pos);
	}
	// Lerp ȸ��
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
	// �⺻ state ����
	//phase = JumpPhase::Ascending;
	m_bOnPlaying = false;
}

BettyTwoHandAttack::BettyTwoHandAttack(weak_ptr<AActor> _pOwner) : StateBase(BETTY_S_A_TWOHAND)
{
	m_pOwner = _pOwner;
	m_bCanInterrupt = false;
}
void BettyTwoHandAttack::Enter()
{
	// �⺻ state ����
	m_bOnPlaying = true;
	// �ִϸ��̼� Idle �÷���
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
	// �⺻ state ����
	m_bOnPlaying = false;
}

BettyOneHandBackAttack::BettyOneHandBackAttack(weak_ptr<AActor> _pOwner) : StateBase(BETTY_S_A_HANDBACK)
{
	m_pOwner = _pOwner;
	m_bCanInterrupt = false;
}
void BettyOneHandBackAttack::Enter()
{
	// �⺻ state ����
	m_bOnPlaying = true;
	// �ִϸ��̼� Idle �÷���
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
	// Lerp ȸ��
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
	// �⺻ state ����
	m_bOnPlaying = false;
}
void BettyOneHandBackAttack::CheckDirection(Vec3 _targetPos)
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
}


BettyOneHandDownAttack::BettyOneHandDownAttack(weak_ptr<AActor> _pOwner) : StateBase(BETTY_S_A_HANDDOWN)
{
	m_pOwner = _pOwner;
	m_bCanInterrupt = false;
}
void BettyOneHandDownAttack::Enter()
{
	// �⺻ state ����
	m_bOnPlaying = true;
	// �ִϸ��̼� Idle �÷���
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
	// Lerp ȸ��
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
	// �⺻ state ����
	m_bOnPlaying = false;
}
void BettyOneHandDownAttack::CheckDirection(Vec3 _targetPos)
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
	// �⺻ init
	m_bOnPlaying = true;

	start->SetDirection(dir);
	middle->SetDirection(dir);
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
		if (rollElapsed > 0.5f)
		{
			rollElapsed = 0.0f;
			bHitWall = true;
		}
		if (bHitWall)
		{
			middle->End(); // ��� end �� ���⼭.
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
	// �⺻ state ����
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
void BettyRollAttack::CheckHitWall(bool _hitWall)
{
	if (currentPhase == RollPhase::Middle)
	{
		bHitWall = _hitWall;
	}
}
#pragma region roll_subState
BettyRollStart::BettyRollStart(weak_ptr<AActor> _pOwner) : StateBase(BETTY_S_A_ROLL)
{
	m_pOwner = _pOwner;
	m_bCanInterrupt = false;
}
void BettyRollStart::Enter()
{
	// �⺻ state ����
	m_bOnPlaying = true;
	// �ִϸ��̼� Idle �÷���
	auto animInstance = m_pOwner.lock()->GetMeshComponent<USkinnedMeshComponent>()->GetAnimInstance();
	int index = animInstance->GetAnimIndex(L"Armature|JumpSpin");
	// spin�� �ٷ� ����ǵ���
	animInstance->SetKeyFrame(index, 17);
	animInstance->PlayOnce(index);
	// ���� ���
	auto look = m_pOwner.lock()->GetLook();
	look.y = 0;
	look.Normalize();
	Vec3 startPos = m_pOwner.lock()->GetPosition();
	Vec3 endPos = startPos + posOffset * look;
	float totalTime = animInstance->GetTotalFrame() / TIMER->GetFPS();
	jumpHandler.Init(startPos, endPos, 8.0f, totalTime, 0.1f, 3.0f);

	// ȸ��
	targetYaw = atan2f(dir.x, dir.z);
}
void BettyRollStart::Tick()
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

	// Lerp ȸ��
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
	// �⺻ state ����
	m_bOnPlaying = false;
}

BettyRollMiddle::BettyRollMiddle(weak_ptr<AActor> _pOwner) : StateBase(BETTY_S_A_ROLL)
{
	m_pOwner = _pOwner;
	m_bCanInterrupt = false;
}
void BettyRollMiddle::Enter()
{
	// �⺻ state ����
	m_bOnPlaying = true;
	// �ִϸ��̼� Idle �÷���
	auto animInstance = m_pOwner.lock()->GetMeshComponent<USkinnedMeshComponent>()->GetAnimInstance();
	int index = animInstance->GetAnimIndex(L"Armature|Spin");
	animInstance->SetCurrentAnimTrack(index);
	// ȸ��
	float targetYaw = atan2f(dir.x, dir.z);
	Vec3 currentRot = m_pOwner.lock()->GetRotation();
	currentRot.y = targetYaw;
	m_pOwner.lock()->SetRotation(currentRot);
}
void BettyRollMiddle::Tick()
{
	// �浹 Ȯ�� �� End ( �ۿ��� ) 
	dir.y = 0;
	dir.Normalize();
	m_pOwner.lock()->AddPosition(dir * 1.2f);
}
void BettyRollMiddle::End()
{
	// �⺻ state ����
	m_bOnPlaying = false;
}

BettyRollEnd::BettyRollEnd(weak_ptr<AActor> _pOwner) : StateBase(BETTY_S_A_ROLL)
{
	m_pOwner = _pOwner;
	m_bCanInterrupt = false;
}
void BettyRollEnd::Enter()
{
	// �⺻ state ����
	m_bOnPlaying = true;
	// �ִϸ��̼� Idle �÷���
	auto animInstance = m_pOwner.lock()->GetMeshComponent<USkinnedMeshComponent>()->GetAnimInstance();
	int index = animInstance->GetAnimIndex(L"Armature|Spin_Collide");
	animInstance->PlayOnce(index);
	// ���� ���
	auto look = m_pOwner.lock()->GetLook();
	look.y = 0;
	look = -look;
	look.Normalize();
	Vec3 startPos = m_pOwner.lock()->GetPosition();
	// �� ? �𸣰���. ���߿� ���� �ʿ� .. 
	startPos.y = 0;
	Vec3 endPos = startPos + posOffset * look;
	float totalTime = animInstance->GetTotalFrame() / TIMER->GetFPS();
	jumpHandler.Init(startPos, endPos, 12.0f, totalTime, 0.1f, 3.0f);

	// ȸ��
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
	// Lerp ȸ��
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
	// �⺻ state ����
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
	// �⺻ state ����
	m_bOnPlaying = true;
	// �ִϸ��̼� Idle �÷���
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
	// �⺻ state ����
	m_bOnPlaying = false;
}

BettyRoarAttack::BettyRoarAttack(weak_ptr<AActor> _pOwner) : StateBase(BETTY_S_A_ROAR)
{
	m_pOwner = _pOwner;
	m_bCanInterrupt = false;
}

void BettyRoarAttack::Enter()
{
	// �⺻ state ����
	m_bOnPlaying = true;
	// �ִϸ��̼� Idle �÷���
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
	// �⺻ state ����
	m_bOnPlaying = false;
}

BettyDeathState::BettyDeathState(weak_ptr<AActor> _pOwner) : StateBase(BETTY_S_DEATH)
{	
	m_pOwner = _pOwner;
	m_bCanInterrupt = false;
}

void BettyDeathState::Enter()
{
	// �⺻ state ����
	m_bOnPlaying = true;
	// �ִϸ��̼� Idle �÷���
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
