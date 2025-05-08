#include "pch.h"
#include "PlayerMoveScript.h"
#include "Input.h"
#include "Timer.h"
#include "APawn.h"
#include "AActor.h"
#include "USkinnedMeshComponent.h"
#include "UAnimInstance.h"

void PlayerMoveScript::Init()
{
	m_vLook = GetOwner()->GetPosition() - GetOwner()->GetCameraComponent()->GetPosition();
	m_pAnimInstance = GetOwner()->GetMeshComponent<USkinnedMeshComponent>()->GetAnimInstance();
}

void PlayerMoveScript::Tick()
{
	float deltaTime = TIMER->GetDeltaTime();

	Vec3 up = { 0, 1, 0 };
	m_vRight = up.Cross(m_vLook);
	m_vLook.y = 0.0f;
	m_vRight.y = 0.0f;

	m_vLook.Normalize();
	m_vRight.Normalize();

	Vec3 moveDir;
	if (INPUT->GetButtonDown(UP))
	{
		moveDir += m_vLook;
	}

	if (INPUT->GetButtonDown(LEFT))
	{
		moveDir += -m_vRight;// * m_fSpeed * deltaTime;
	}

	if (INPUT->GetButtonDown(DOWN))
	{
		moveDir += -m_vLook;// * m_fSpeed * deltaTime;
	}

	if (INPUT->GetButtonDown(RIGHT))
	{
		moveDir += m_vRight;// * m_fSpeed * deltaTime;
	}

	if (INPUT->GetButtonDown(LCLICK))
	{
		m_bAttack  = true;
		int targetIndex = m_pAnimInstance->GetAnimIndex(L"Slash_Light_R_new");
		m_pAnimInstance->PlayOnce(targetIndex);
	}
	else
	{
		m_bAttack = false;
	}

	if (moveDir.Length() > 0 && !m_pAnimInstance->m_bOnPlayOnce)
	{
		// �ִ� ( ���� ó�� ���� ������Ʈ �ʿ� )
		{
			int targetIndex = m_pAnimInstance->GetAnimIndex(L"Run");
			m_pAnimInstance->SetCurrentAnimTrack(targetIndex);
		}

		// �̵�
		{
			moveDir.Normalize();
			Vec3 pos = moveDir * m_fSpeed * deltaTime;
			GetOwner()->AddPosition(pos);
		}

		// ȸ��		
		{
			float targetYaw = atan2f(moveDir.x, moveDir.z);
			Vec3 currentRot = GetOwner()->GetRotation();
			float currentYaw = currentRot.y;

			// ���� ���� ��� (-�� ~ �� ������)
			float angleDiff = targetYaw - currentYaw;
			while (angleDiff > DD_PI)  angleDiff -= DD_PI * 2;
			while (angleDiff < -DD_PI) angleDiff += DD_PI * 2;

			// Lerp ���
			float smoothedYaw = currentRot.y + angleDiff * m_fRotationSpeed * deltaTime;

			currentRot.y = smoothedYaw;
			GetOwner()->SetRotation(currentRot);
		}
	}
	else
	{
		int targetIndex = m_pAnimInstance->GetAnimIndex(L"Idle_0");
		m_pAnimInstance->SetCurrentAnimTrack(targetIndex);
	}

}
