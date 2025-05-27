#include "pch.h"
#include "BatMovement.h"
#include "Timer.h"
#include <algorithm>

void BatMovement::Init()
{
	m_vCenter = GetOwner()->GetPosition();
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

	Vec3 direction = m_vCenter - GetOwner()->GetPosition();
	direction.y = 0;
	direction.Normalize();

	GetOwner()->SetPosition(pos);

	// direction을 이용한 회전
	Vec3 tempUp = { 0.0f, 1.0f, 0.0f };
	Vec3 moveDir = tempUp.Cross(direction); // 반시계 방향

	float targetYaw = atan2f(moveDir.x, moveDir.z);
	Vec3 currentRot = GetOwner()->GetRotation();
	currentRot.y = targetYaw;
	GetOwner()->SetRotation(currentRot);

	{
		// 얘는 왜 smooth가 필요가 없지 ? 매 프레임 이동해서 ?
		//float currentYaw = currentRot.y;
		//// 각도 차이 계산
		//float angleDiff = targetYaw - currentYaw;
		//while (angleDiff > DD_PI)  angleDiff -= DD_PI * 2;
		//while (angleDiff < -DD_PI) angleDiff += DD_PI * 2;
		//// Lerp 계산
		//// 5.0 이 아니면 아예 회전 방향이 이상해짐 .. . . .. 왜 ? 
		//float smoothedYaw = currentRot.y + angleDiff * 5.0f * deltaTime;
		//currentRot.y = smoothedYaw;
		//GetOwner()->SetRotation(currentRot);
	}


}
