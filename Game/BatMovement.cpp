#include "pch.h"
#include "BatMovement.h"
#include "Timer.h"
#include <algorithm>

void BatMovement::Init()
{
	// Set Center
	m_vCenter = GetOwner()->GetPosition();
	m_vCenter.z -= m_fRadius;
	Vec3 currentRot = GetOwner()->GetRotation();

	// Init Rotation
	{
		// direction�� �̿��� ȸ��
		Vec3 tempUp = { 0.0f, 1.0f, 0.0f };
		Vec3 tempDir = { -1.0f,  0.0f, 0.0f };

		// ȸ��		

		float targetYaw = atan2f(tempDir.x, tempDir.z);
		Vec3 currentRot = GetOwner()->GetRotation();
		float currentYaw = currentRot.y;
		currentRot.y = targetYaw;

		GetOwner()->SetRotation(currentRot);
	}
}


void BatMovement::Tick()
{
	float deltaTime = TIMER->GetDeltaTime();
	angle += rotateSpeed * deltaTime;

	// 2PI ������ �ǵ���
	if (angle > 2 * DD_PI) angle -= 2 * DD_PI;

	// ��ġ ����
	Vec3 pos;
	pos.x = m_vCenter.x + m_fRadius * std::cos(angle);
	pos.z = m_vCenter.z + m_fRadius * std::sin(angle);

	Vec3 direction = m_vCenter - GetOwner()->GetPosition();
	direction.y = 0;
	direction.Normalize();

	GetOwner()->SetPosition(pos);

	// direction�� �̿��� ȸ��
	Vec3 tempUp = { 0.0f, 1.0f, 0.0f };
	Vec3 moveDir = tempUp.Cross(direction); // �ݽð� ����

	// ȸ��		
	{
		float targetYaw = atan2f(moveDir.x, moveDir.z);
		Vec3 currentRot = GetOwner()->GetRotation();
		float currentYaw = currentRot.y;

		// ���� ���� ���
		float angleDiff = targetYaw - currentYaw;
		while (angleDiff > DD_PI)  angleDiff -= DD_PI * 2;
		while (angleDiff < -DD_PI) angleDiff += DD_PI * 2;

		// Lerp ���
		// 5.0 �� �ƴϸ� �ƿ� ȸ�� ������ �̻����� .. . . .. �� ? 
		float smoothedYaw = currentRot.y + angleDiff * 5.0f * deltaTime;

		currentRot.y = smoothedYaw;
		GetOwner()->SetRotation(currentRot);
	}


}
