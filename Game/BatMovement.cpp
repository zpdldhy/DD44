#include "pch.h"
#include "BatMovement.h"
#include "Timer.h"
#include <algorithm>
#include "EffectManager.h"
#include "UMeshComponent.h"
// temp
#include "Input.h"

void BatMovement::Init()
{
	m_vCenter = GetOwner()->GetPosition();

	// FSM
	idle = make_shared<BatIdleState>(m_pOwner);
	walk = make_shared<BatWalkState>(m_pOwner);
	attack = make_shared<BatAttackState>(m_pOwner);
	death = make_shared<BatDieState>(m_pOwner);

	currentState = idle;
	currentState->Enter();
}


void BatMovement::Tick()
{
	Flashing();

	currentState->Tick();
	if (currentState->GetId() == ENEMY_STATE::ENEMY_S_DEATH)
	{
		if (!currentState->IsPlaying())
		{
			// bat 죽음
			GetOwner()->m_bDelete = true;
			//GetOwner()->GetMeshComponent()->SetVisible(false);
		}
		return;
	}

	if (currentState->GetId() != ENEMY_STATE::ENEMY_S_DEATH)
	{
		Vec3 distance = player.lock()->GetPosition() - GetOwner()->GetPosition();
		if (distance.Length() < 3.0f && INPUT->GetButton(LCLICK))
		{
			// Blood FX
			Vec3 basePos = GetOwner()->GetPosition();
			basePos.y += RandomRange(3, 4);
			Vec3 look = GetOwner()->GetLook();
			velocity = -look;
			PlayBloodBurst(basePos, velocity, 25.0f, 90.0f);


			m_fHitFlashTimer = 1.f;  // 1초 동안
			m_bIsFlashing = true;

			ChangetState(death);
			return;
		}
	}
	if (m_bReturn)
	{
		ReturningToPos();
		return;
	}
	if (currentState->GetId() != ENEMY_STATE::ENEMY_S_ATTACK)
	{
		float deltaTime = TIMER->GetDeltaTime();
		{
			// 획일성을 줄이기 위해 
			rotateSpeed = RandomRange((originRotateSpeed - 0.005f), (originRotateSpeed + 0.01f));

		}
		angle += rotateSpeed * deltaTime;

		// 2PI 넘으면 되돌림
		if (angle > 2 * DD_PI) angle -= 2 * DD_PI;

		// 위치 갱신
		Vec3 pos;
		pos.x = m_vCenter.x + m_fRadius * std::cos(angle);
		pos.z = m_vCenter.z + m_fRadius * std::sin(angle);
		GetOwner()->SetPosition(pos);

		Vec3 direction = m_vCenter - GetOwner()->GetPosition();
		direction.y = 0;
		direction.Normalize();

		// direction을 이용한 회전
		Vec3 tempUp = { 0.0f, 1.0f, 0.0f };
		Vec3 moveDir = tempUp.Cross(direction); // 반시계 방향

		// 회전 TargetPos 바라보기
		float targetYaw = atan2f(moveDir.x, moveDir.z);
		Vec3 currentRot = GetOwner()->GetRotation();
		currentRot.y = targetYaw;
		GetOwner()->SetRotation(currentRot);

		//
		Vec3 distance = player.lock()->GetPosition() - GetOwner()->GetPosition();

		if (distance.Length() < 5.0f && distance.Length() > 3.0f)
		{
			m_bCanStartAttack = true;
			m_vTargetPos = player.lock()->GetPosition();
		}


		Attack();
	}
	else
	{
		// 공격에 맞춘 이동
		m_fLerp += m_fLerpOffset;
		Vec3 diff = m_vTargetPos - GetOwner()->GetPosition();
		Vec3 pos = diff * m_fLerp;

		GetOwner()->AddPosition(pos);

		if (!currentState->IsPlaying())
		{
			// ReturnPos 찾기
			// 시도 1 : 가장 가까운 원 위의 한 점으로 이동
			{
				//Vec3 dir = m_vCenter - GetOwner()->GetPosition();
				//Vec3 rDir = dir;
				//rDir.Normalize();
				//Vec3 temp = rDir * m_fRadius;

				//Vec3 diff = dir - temp;
				//m_vReturnPos = GetOwner()->GetPosition() + diff;
			}
			// 시도 2 : 다른 접점으로 이동
			{
				Vec3 d = GetOwner()->GetPosition() - m_vCenter;
				float dLength = d.Length();
				Vec3 d_norm = d / dLength;

				Vec3 tempUp = Vec3(0, 1, 0);
				Vec3 t1 = d_norm.Cross(tempUp);

				m_vReturnPos = m_vCenter + t1 * m_fRadius;
			}

			// 
			m_bReturn = true;
			ChangetState(walk);
			m_fLerp = -0.05;
		}
	}


}

void BatMovement::ChangetState(shared_ptr<StateBase> _state)
{

	if (!currentState->IsInterruptible() && currentState->IsPlaying())
	{
		if (!_state->GetId() == ENEMY_S_DEATH)
		{
			return;
		}
	}

	if (currentState)
		currentState->End();

	currentState = _state;

	if (currentState)
		currentState->Enter();
}

void BatMovement::ReturningToPos()
{
	Vec3 ownerPos = GetOwner()->GetPosition();
	Vec3 diff = m_vReturnPos - ownerPos;
	if (abs(diff.x) <= 0.5f || abs(diff.z) <= 0.5f)
	{
		m_bReturn = false;
	}

	Vec3 pos = diff * 0.005f;
	GetOwner()->AddPosition(pos);

	Vec3 currentPos = GetOwner()->GetPosition();
	Vec3 dir = currentPos - m_vCenter;
	dir.y = 0; // y는 무시

	angle = std::atan2(dir.z, dir.x);

	// 회전 TargetPos 바라보기
	Vec3 lookDir = m_vReturnPos - ownerPos;
	float targetYaw = atan2f(lookDir.x, lookDir.z);
	Vec3 currentRot = GetOwner()->GetRotation();
	float currentYaw = currentRot.y;

	// 각도 차이 계산
	float angleDiff = targetYaw - currentYaw;
	while (angleDiff > DD_PI)  angleDiff -= DD_PI * 2;
	while (angleDiff < -DD_PI) angleDiff += DD_PI * 2;

	// Lerp 계산
	float smoothedYaw = currentRot.y + angleDiff * m_fRotationSpeed * TIMER->GetDeltaTime();

	currentRot.y = smoothedYaw;
	GetOwner()->SetRotation(currentRot);
}

void BatMovement::PlayBloodBurst(const Vec3& _origin, const Vec3& _direction, float _speed, float _spreadAngleDeg, int _minCount, int _maxCount)
{
	int count = RandomRange(_minCount, _maxCount);
	for (int i = 0; i < count; ++i)
	{
		Vec3 offset = Vec3(RandomRange(-0.3f, 0.3f), RandomRange(-0.3f, 0.3f), RandomRange(-0.3f, 0.3f));
		Vec3 pos = _origin + offset;

		Vec3 baseVelocity = _direction * _speed;
		EFFECT->PlayEffect(EEffectType::Blood, pos, _spreadAngleDeg, baseVelocity);
	}
}
// 빈 함수, 기능 필요하면 넣기용.
void BatMovement::VisitAllMeshMaterials(shared_ptr<UMeshComponent> comp)
{
	if (!comp) return;

	shared_ptr<UMaterial> mat = comp->GetMaterial();
	if (mat)
	{

	}

	for (int i = 0; i < comp->GetChildCount(); ++i)
	{
		VisitAllMeshMaterials(comp->GetChild(i));
	}
}

void BatMovement::ApplyHitFlashToAllMaterials(shared_ptr<UMeshComponent> comp, float value)
{
	if (!comp) return;

	shared_ptr<UMaterial> mat = comp->GetMaterial();
	if (mat)
	{
		mat->SetHitFlashTime(value); // CB에 g_fHitFlashTime 전달
	}

	for (int i = 0; i < comp->GetChildCount(); ++i)
	{
		ApplyHitFlashToAllMaterials(comp->GetChild(i), value);
	}
}

void BatMovement::Flashing()
{
	if (m_bIsFlashing)
	{
		m_fHitFlashTimer -= TIMER->GetDeltaTime();
		if (m_fHitFlashTimer <= 0.0f)
		{
			m_fHitFlashTimer = 0.0f;
			m_bIsFlashing = false;
		}

		// hitFlashAmount는 1 → 0 으로 감소
		float hitFlashAmount = std::min(std::max<float>(m_fHitFlashTimer, 0.0f), 1.0f);

		auto root = GetOwner()->GetMeshComponent();
		ApplyHitFlashToAllMaterials(root, hitFlashAmount);
	}
}

void BatMovement::Attack()
{
	if (m_bCanStartAttack) // 실제 로직에선 플레이어 인식 후 targetPos 업데이트
	{
		// 이 부분 처리 개선 필요
		if (!currentState->IsInterruptible() && currentState->IsPlaying())
		{
			return;
		}
		m_vPrevPos = GetOwner()->GetPosition();
		//m_vTargetPos = m_vPrevPos + GetOwner()->GetLook() * 5.0f;
		Vec3 diff = m_vTargetPos - GetOwner()->GetPosition();

		// 회전 TargetPos 바라보기
		float targetYaw = atan2f(diff.x, diff.z);
		Vec3 currentRot = GetOwner()->GetRotation();
		currentRot.y = targetYaw;
		GetOwner()->SetRotation(currentRot);

		ChangetState(attack);
		m_bCanStartAttack = false;
	}
}
