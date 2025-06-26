#include "pch.h"
#include "EnemyScriptBase.h"
#include "StateBase.h"
#include "EnemyState.h"
#include "Timer.h"

void EnemyScriptBase::ChangeState(shared_ptr<StateBase> _state)
{
	// �ߺ� ����
	if (currentState == _state)
	{
		return;
	}

	// ���� Ȯ��
	if (currentState && !currentState->IsInterruptible() && currentState->IsPlaying())
	{
		if (!(_state->GetId() == static_cast<UINT>(ENEMY_S_DEATH)))
		{
			return;
		}
	}

	// ���� ����
	if (currentState)
	{
		currentState->End();
	}

	// ���� ����
	currentState = _state;
	currentState->Enter();
	currentStateId = currentState->GetId();
}

bool EnemyScriptBase::LerpRotate(float _targetYaw)
{
	Vec3 currentRot = GetOwner()->GetRotation();
	float currentYaw = currentRot.y;

	// ���� ���� ���
	float angleDiff = _targetYaw - currentYaw;
	while (angleDiff > DD_PI)  angleDiff -= DD_PI * 2;
	while (angleDiff < -DD_PI) angleDiff += DD_PI * 2;

	// �Ϸ� Ȯ��
	const float angleThreshold = 0.01f;

	if (std::abs(angleDiff) < angleThreshold)
	{
		currentRot.y = _targetYaw;
		GetOwner()->SetRotation(currentRot);
		return true;
	}

	// Lerp ���
	float smoothedYaw = currentRot.y + angleDiff * m_fRotationSpeed * TIMER->GetDeltaTime();
	currentRot.y = smoothedYaw;
	GetOwner()->SetRotation(currentRot);
	return false;
}
