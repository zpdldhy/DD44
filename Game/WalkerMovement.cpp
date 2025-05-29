#include "pch.h"
#include "WalkerMovement.h"

#include "AActor.h"
#include "EffectManager.h"

#include "Input.h"
#include "Timer.h"


void WalkerMovement::Init()
{
	// position
	float hypotenuse = sqrt(2 *m_half * m_half);
	auto p = GetOwner()->GetPosition();
	m_vPos[0] = GetOwner()->GetPosition() - m_half * Vec3(1, 0, 1);
	m_vPos[1] = GetOwner()->GetPosition() - m_half * Vec3(1, 0, -1);
	m_vPos[2] = GetOwner()->GetPosition() - m_half * Vec3(-1, 0, -1);
	m_vPos[3] = GetOwner()->GetPosition() - m_half * Vec3(-1, 0, 1);

	m_currentPosIdx = (int)RandomRange(0.0f, 3.0f);
	m_vCurrentTarget = m_vPos[m_currentPosIdx];
	m_vDistance = m_vCurrentTarget - GetOwner()->GetPosition();

	// 회전 
	Vec3 direction = GetOwner()->GetPosition() - m_vCurrentTarget;
	direction.y = 0;
	direction.Normalize();
	Vec3 tempUp = { 0.0f, 1.0f, 0.0f };
	Vec3 moveDir = tempUp.Cross(direction); // 반시계 방향
	float targetYaw = atan2f(moveDir.x, moveDir.z);
	Vec3 currentRot = GetOwner()->GetRotation();
	currentRot.y = targetYaw;
	GetOwner()->SetRotation(currentRot);

	// state
	idle = make_shared<WalkerIdleState>(GetOwner());
	walk = make_shared<WalkerWalkState>(GetOwner());
	hit = make_shared<WalkerHitState>(GetOwner());
	death = make_shared<WalkerDieState>(GetOwner());

	currentState = walk;
	currentState->Enter();
}

void WalkerMovement::Tick()
{
	Flashing();
	LerpRotate();
	currentState->Tick();
	if (currentState->GetId() == ENEMY_STATE::ENEMY_S_DEATH)
	{
		if (!currentState->IsPlaying())
		{
			// 죽음
			GetOwner()->m_bDelete = true;
		}
		return;
	}
	if (currentState->GetId() == ENEMY_STATE::ENEMY_S_HIT)
	{
		if (!currentState->IsPlaying())
		{
			m_rotate = true;
			ChangeState(walk);
		}
		return;
	}

	// 걷는 상황일 때 위치 조절해가며 걷기
	if (currentState->GetId() == ENEMY_STATE::ENEMY_S_WALK)
	{
		Vec3 pos = GetOwner()->GetPosition();
		Vec3 diff = m_vCurrentTarget - pos;
		if (diff.Length() < 0.1f)
		{
			// 다음 목표 설정
			m_currentPosIdx = (++m_currentPosIdx) % 4;
			m_vCurrentTarget = m_vPos[m_currentPosIdx];
			m_vDistance = m_vCurrentTarget - GetOwner()->GetPosition();

			// 회전 
			Vec3 direction = GetOwner()->GetPosition() - m_vCurrentTarget;
			direction.y = 0;
			direction.Normalize();

			// direction을 이용한 회전
			Vec3 tempUp = { 0.0f, 1.0f, 0.0f };
			moveDir = tempUp.Cross(direction); // 반시계 방향
			m_rotate = true;
		}
		else
		{
			// 이동
			Vec3 pos = m_vDistance * m_walkSpeed;
			GetOwner()->AddPosition(pos);
		}

	}

	Vec3 distance = player.lock()->GetPosition() - GetOwner()->GetPosition();

	if (distance.Length() < 3.0f && INPUT->GetButton(LCLICK))
	{
		// Blood FX
		Vec3 basePos = GetOwner()->GetPosition();
		basePos.y += RandomRange(0.5, 2);
		Vec3 look = GetOwner()->GetLook();
		velocity = -look;
		PlayBloodBurst(basePos, velocity, 50.0f, 90.0f);
		
		// Flash FX
		m_fHitFlashTimer = 1.f;  // 1초 동안
		m_bIsFlashing = true;

		// 회전 
		Vec3 direction = GetOwner()->GetPosition() - player.lock()->GetPosition();
		direction.y = 0;
		direction.Normalize();
		Vec3 tempUp = { 0.0f, 1.0f, 0.0f };
		Vec3 moveDir = tempUp.Cross(direction); // 반시계 방향
		float targetYaw = atan2f(moveDir.x, moveDir.z);
		Vec3 currentRot = GetOwner()->GetRotation();
		currentRot.y = targetYaw;
		GetOwner()->SetRotation(currentRot);
		m_rotate = false;

		// Status
		m_hp--;
		if (m_hp <= 0)
		{
			ChangeState(death);
			GetOwner()->AddPosition(Vec3(0.0f, -0.8f, 0.0f));
			return;
		}

		// Anim State
		ChangeState(hit);
	}

}

void WalkerMovement::ChangeState(shared_ptr<StateBase> _state)
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

void WalkerMovement::PlayBloodBurst(const Vec3& _origin, const Vec3& _direction, float _speed, float _spreadAngleDeg, int _minCount, int _maxCount)
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
void WalkerMovement::VisitAllMeshMaterials(shared_ptr<UMeshComponent> comp)
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

void WalkerMovement::ApplyHitFlashToAllMaterials(shared_ptr<UMeshComponent> comp, float value)
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

void WalkerMovement::Flashing()
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

void WalkerMovement::LerpRotate()
{
	if(!m_rotate) return;
	float targetYaw = atan2f(moveDir.x, moveDir.z);
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
