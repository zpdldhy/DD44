#include "pch.h"
#include "BatMovement.h"
#include "Timer.h"
#include <algorithm>

void BatMovement::Init()
{
	// Set Center
	m_vCenter = GetOwner()->GetPosition();
	Vec3 currentRot = GetOwner()->GetRotation();
}


void BatMovement::Tick()
{
	float deltaTime = TIMER->GetDeltaTime();
	angle += rotateSpeed * deltaTime;

	// 2PI 넘으면 되돌림
	if (angle > 2 * DD_PI) angle -= 2 * DD_PI;

	// 위치 갱신
	Vec3 pos;
	pos.x = m_vCenter.x + m_fRadius * std::cos(angle);
	pos.z = m_vCenter.z + m_fRadius * std::sin(angle);

	Vec3 direction = GetOwner()->GetPosition() - pos;
	//direction.z = abs(direction.z);
	//direction.x = abs(direction.x);
	direction.y = 0;
	direction.Normalize();


	GetOwner()->SetPosition(pos);

	// direction을 이용한 회전
	Vec3 tempUp = { 0.0f, 1.0f, 0.0f };
	Vec3 moveDir = direction.Cross(tempUp);

	// 회전		
	{
		float targetYaw = atan2f(moveDir.x, moveDir.z);
		Vec3 currentRot = GetOwner()->GetRotation();
		float currentYaw = currentRot.y;

		// 각도 차이 계산
		float angleDiff = targetYaw - currentYaw;
		while (angleDiff > DD_PI)  angleDiff -= DD_PI * 2;
		while (angleDiff < -DD_PI) angleDiff += DD_PI * 2;

		// Lerp 계산
		float smoothedYaw = currentRot.y + angleDiff * 7.0f * deltaTime;

		currentRot.y = smoothedYaw;
		GetOwner()->SetRotation(currentRot);
	}


}
