#include "pch.h"
#include "HeadRollerStates.h"
#include "AActor.h"
#include "UInstanceSkinnedMeshComponent.h"
#include "UAnimInstance.h"
#include "Timer.h"

#include "ObjectManager.h"
#include "CollisionManager.h"
#include "UBoxComponent.h"

HeadRollerIdleState::HeadRollerIdleState(weak_ptr<AActor> _pOwner) : StateBase(ENEMY_S_IDLE)
{
	m_pOwner = _pOwner;
	m_bCanInterrupt = true;
}
void HeadRollerIdleState::Enter()
{
	// 기본 state 세팅
	m_bOnPlaying = true;
	// 애니메이션 Idle 플레이
	auto animInstance = m_pOwner.lock()->GetMeshComponent<USkinnedMeshComponent>()->GetAnimInstance();
	int idleIndex = animInstance->GetAnimIndex(L"Armature|QuickLook");
	animInstance->SetCurrentAnimTrack(idleIndex);
}
void HeadRollerIdleState::Tick()
{
}
void HeadRollerIdleState::End()
{
	// 기본 state 세팅
	m_bOnPlaying = false;
}

HeadRollerAttackState::HeadRollerAttackState(weak_ptr<AActor> _pOwner) : StateBase(ENEMY_S_ATTACK)
{
	m_pOwner = _pOwner;
	m_bCanInterrupt = false;

	find = make_shared<HeadRollerRollFind>(m_pOwner);
	start = make_shared<HeadRollerRollStart>(m_pOwner);
	middle = make_shared<HeadRollerRollMiddle>(m_pOwner);
	end = make_shared<HeadRollerRollEnd>(m_pOwner);
}

void HeadRollerAttackState::Enter()
{
	// 기본 state 세팅
	m_bOnPlaying = true;

	// sub-state 세팅
	find->SetDirection(dir);
	start->SetDirection(dir);
	middle->SetDirection(dir);
	end->SetDirection(dir);

	find->Enter();

	m_pOwner.lock()->m_szName = L"Enemy";
}

void HeadRollerAttackState::Tick()
{
	switch (currentPhase)
	{
	case RollPhase::Find: {
		find->Tick();
		if (!find->IsPlaying())
		{
			start->Enter();
			currentPhase = RollPhase::Start;
		}
		break;
	}
	case RollPhase::Start: {
		start->Tick();
		if (!start->IsPlaying())
		{
			middle->Enter();
			currentPhase = RollPhase::Middle;
		}
		break;
	}
	case RollPhase::Middle: {
		middle->Tick();
		rollElapsed += TIMER->GetDeltaTime();
		if (CheckWallCollision())
		{
			middle->End();
			end->Enter();
			currentPhase = RollPhase::Final;
		}
		break;
	}
	case RollPhase::Final: {
		end->Tick();
		if (!end->IsPlaying())
		{
			currentPhase = RollPhase::Done;
		}
		break;
	}
	case RollPhase::Done: {
		End();
		break;
	}
	}
}

void HeadRollerAttackState::End()
{
	// 기본 state 세팅
	m_bOnPlaying = false;
	// 변수 초기화
	currentPhase = RollPhase::Find;
	m_pOwner.lock()->m_szName = L"HeadRoller";
}

bool HeadRollerAttackState::CheckWallCollision()
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
	Vec3 range(6.0f, 0.0f, 6.0f);

	for (const auto& colData : m_pOwner.lock()->m_vCollisionList)
	{
		if (OBJECT->GetActor(colData.first)->m_szName == L"MyCharacter")
		{
			continue;
		}
		auto targetShape = OBJECT->GetActor(colData.first)->GetShapeComponent();
		auto targetBox = dynamic_pointer_cast<UBoxComponent>(targetShape);

		// 걍 뭔가 충돌하면 다 처리해버려 ? 

		return true;

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

#pragma region roll sub-state
HeadRollerRollFind::HeadRollerRollFind(weak_ptr<AActor> _pOwner) : StateBase(ENEMY_S_ATTACK)
{
	m_pOwner = _pOwner;
	m_bCanInterrupt = false;
}
void HeadRollerRollFind::Enter()
{
	// 기본 state 세팅
	m_bOnPlaying = true;

	// 애니메이션 Walk 플레이
	auto animInstance = m_pOwner.lock()->GetMeshComponent<USkinnedMeshComponent>()->GetAnimInstance();
	int idleIndex = animInstance->GetAnimIndex(L"Armature|StartleHop");
	animInstance->PlayOnce(idleIndex);

	// 회전할 방향 정하기
	targetYaw = atan2f(dir.x, dir.z);
}
void HeadRollerRollFind::Tick()
{
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

	// 애니메이션
	auto animInstance = m_pOwner.lock()->GetMeshComponent<USkinnedMeshComponent>()->GetAnimInstance();
	if (!animInstance->m_bOnPlayOnce)
	{
		End();
	}
}
void HeadRollerRollFind::End()
{
	// 기본 state 세팅
	m_bOnPlaying = false;
}

HeadRollerRollStart::HeadRollerRollStart(weak_ptr<AActor> _pOwner) : StateBase(ENEMY_S_ATTACK)
{
	m_pOwner = _pOwner;
	m_bCanInterrupt = false;
}
void HeadRollerRollStart::Enter()
{
	// 기본 state 세팅
	m_bOnPlaying = true;

	// 애니메이션 Walk 플레이
	auto animInstance = m_pOwner.lock()->GetMeshComponent<USkinnedMeshComponent>()->GetAnimInstance();
	int idleIndex = animInstance->GetAnimIndex(L"Armature|roll_start");
	animInstance->PlayOnce(idleIndex);
}
void HeadRollerRollStart::Tick()
{
	auto animInstance = m_pOwner.lock()->GetMeshComponent<USkinnedMeshComponent>()->GetAnimInstance();
	if (!animInstance->m_bOnPlayOnce)
	{
		End();
	}
}
void HeadRollerRollStart::End()
{
	// 기본 state 세팅
	m_bOnPlaying = false;
}

HeadRollerRollMiddle::HeadRollerRollMiddle(weak_ptr<AActor> _pOwner) : StateBase(ENEMY_S_ATTACK)
{
	m_pOwner = _pOwner;
	m_bCanInterrupt = false;
}
void HeadRollerRollMiddle::Enter()
{
	// 기본 state 세팅
	m_bOnPlaying = true;

	// 애니메이션 Walk 플레이
	auto animInstance = m_pOwner.lock()->GetMeshComponent<USkinnedMeshComponent>()->GetAnimInstance();
	int idleIndex = animInstance->GetAnimIndex(L"Armature|roll_loop");
	animInstance->SetCurrentAnimTrack(idleIndex);
}
void HeadRollerRollMiddle::Tick()
{
	// 이동
	m_pOwner.lock()->AddPosition(dir);
}
void HeadRollerRollMiddle::End()
{
	// 기본 state 세팅
	m_bOnPlaying = false;
}

HeadRollerRollEnd::HeadRollerRollEnd(weak_ptr<AActor> _pOwner) : StateBase(ENEMY_S_ATTACK)
{
	m_pOwner = _pOwner;
	m_bCanInterrupt = false;
}
void HeadRollerRollEnd::Enter()
{
	// 기본 state 세팅
	m_bOnPlaying = true;

	// 애니메이션 Walk 플레이
	auto animInstance = m_pOwner.lock()->GetMeshComponent<USkinnedMeshComponent>()->GetAnimInstance();
	int index = animInstance->GetAnimIndex(L"Armature|Stun");
	animInstance->SetKeyFrame(index, 65);
	animInstance->PlayOnce(index);
}
void HeadRollerRollEnd::Tick()
{
	auto animInstance = m_pOwner.lock()->GetMeshComponent<USkinnedMeshComponent>()->GetAnimInstance();
	// Stun 애니메이션 ( stun + standUp )
	if (animInstance->GetCurrentFrame() < 24)
	{
		m_pOwner.lock()->AddPosition(-dir * 0.25f);
	}
	if (!animInstance->m_bOnPlayOnce)
	{
		End();
	}
}
void HeadRollerRollEnd::End()
{
	// 기본 state 세팅
	m_bOnPlaying = false;
}
#pragma endregion

HeadRollerLookState::HeadRollerLookState(weak_ptr<AActor> _pOwner) : StateBase(ENEMY_S_LOOK)
{
	m_pOwner = _pOwner;
	m_bCanInterrupt = true;
}

void HeadRollerLookState::Enter()
{
	// 기본 state 세팅
	m_bOnPlaying = true;
	// 애니메이션 Idle 플레이
	auto animInstance = m_pOwner.lock()->GetMeshComponent<USkinnedMeshComponent>()->GetAnimInstance();
	int idleIndex = animInstance->GetAnimIndex(L"Armature|QuickLook");
	animInstance->PlayOnce(idleIndex);
}

void HeadRollerLookState::Tick()
{
	auto animInstance = m_pOwner.lock()->GetMeshComponent<USkinnedMeshComponent>()->GetAnimInstance();
	if (!animInstance->m_bOnPlayOnce)
	{
		End();
	}
}

void HeadRollerLookState::End()
{
	// 기본 state 세팅
	m_bOnPlaying = false;
}

HeadRollerDieState::HeadRollerDieState(weak_ptr<AActor> _pOwner) : StateBase(ENEMY_S_DEATH)
{
	m_pOwner = _pOwner;
	m_bCanInterrupt = false;
}

void HeadRollerDieState::Enter()
{
	// 기본 state 세팅
	m_bOnPlaying = true;
	// 애니메이션 Idle 플레이
	auto animInstance = m_pOwner.lock()->GetMeshComponent<USkinnedMeshComponent>()->GetAnimInstance();
	int index = animInstance->GetAnimIndex(L"Armature|Stun");
	animInstance->SetKeyFrame(index, 29);
	animInstance->m_fAnimPlayRate = 20.0f;
	animInstance->PlayOnce(index);

	// 눈을 먼저 지워버리자
	auto eye1 = m_pOwner.lock()->GetMeshComponent()->GetChildByName(L"Eye1");
	auto eye2 = m_pOwner.lock()->GetMeshComponent()->GetChildByName(L"Eye2");

	eye1->SetVisible(false);
	eye2->SetVisible(false);


}

void HeadRollerDieState::Tick()
{
	auto animInstance = m_pOwner.lock()->GetMeshComponent<USkinnedMeshComponent>()->GetAnimInstance();

	if (!animInstance->m_bOnPlayOnce)
	{
		// 종료
		animInstance->m_bPlay = false;
		currentPhase = STAYSTILL;
		End();
	}

	float frameTime = animInstance->GetTotalFrame();
	frameTime /= 30;
	m_fDissolveTimer += TIMER->GetDeltaTime();
	float t = m_fDissolveTimer / frameTime;
	auto comp = m_pOwner.lock()->GetMeshComponent();

	ApplyDissolveToAllMaterials(comp, t);
}

void HeadRollerDieState::ApplyDissolveToAllMaterials(shared_ptr<UMeshComponent> _comp, float _time)
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

void HeadRollerDieState::SetShadowVisible(const shared_ptr<class UMeshComponent>& _comp)
{
	auto mesh = dynamic_pointer_cast<UInstanceSkinnedMeshComponent>(_comp);
	if (mesh)
	{
		mesh->SetVisible(false);
	}

	for (int i = 0; i < _comp->GetChildCount(); ++i)
	{
		SetShadowVisible(_comp->GetChild(i));
	}
}


void HeadRollerDieState::End()
{
	// 기본 state 세팅
	m_bOnPlaying = false;
	currentPhase = DiePhase::PLAYANIM;
	currentTime = 0.0f;

}
