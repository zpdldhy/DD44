#include "pch.h"
#include "EnemyScriptBase.h"
#include "StateBase.h"
#include "EnemyState.h"
#include "Timer.h"

void EnemyScriptBase::ChangeState(shared_ptr<StateBase> _state)
{
	// 중복 무시
	if (currentState == _state)
	{
		return;
	}

	// 상태 확인
	if (currentState && !currentState->IsInterruptible() && currentState->IsPlaying())
	{
		if (!(_state->GetId() == static_cast<UINT>(ENEMY_S_DEATH)))
		{
			return;
		}
	}

	// 상태 종료
	if (currentState)
	{
		currentState->End();
	}

	// 상태 전이
	currentState = _state;
	currentState->Enter();
	currentStateId = currentState->GetId();
}

bool EnemyScriptBase::LerpRotate(float _targetYaw)
{
	Vec3 currentRot = GetOwner()->GetRotation();
	float currentYaw = currentRot.y;

	// 각도 차이 계산
	float angleDiff = _targetYaw - currentYaw;
	while (angleDiff > DD_PI)  angleDiff -= DD_PI * 2;
	while (angleDiff < -DD_PI) angleDiff += DD_PI * 2;

	// 완료 확인
	const float angleThreshold = 0.01f;

	if (std::abs(angleDiff) < angleThreshold)
	{
		currentRot.y = _targetYaw;
		GetOwner()->SetRotation(currentRot);
		return true;
	}

	// Lerp 계산
	float smoothedYaw = currentRot.y + angleDiff * m_fRotationSpeed * TIMER->GetDeltaTime();
	currentRot.y = smoothedYaw;
	GetOwner()->SetRotation(currentRot);
	return false;
}
