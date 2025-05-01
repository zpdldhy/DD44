#include "pch.h"
#include "kkongchiMoveScript.h"
#include "Input.h"
#include "Timer.h"

void kkongchiMoveScript::Tick()
{
	float deltaTime = TIMER->GetDeltaTime() * 10.f;

	auto vUp = m_pOwner->GetLook() - m_pOwner->GetRight();
	auto vRight = m_pOwner->GetLook() + m_pOwner->GetRight();

	vUp.Normalize();
	vRight.Normalize();

	if (INPUT->GetButtonDown(LSHIFT))
		deltaTime *= 10.f;

	if (INPUT->GetButtonDown(UP))
		m_pOwner->AddPosition(vUp * deltaTime);

	if (INPUT->GetButtonDown(LEFT))
		m_pOwner->AddPosition(-vRight * deltaTime);

	if (INPUT->GetButtonDown(DOWN))
		m_pOwner->AddPosition(-vUp * deltaTime);

	if (INPUT->GetButtonDown(RIGHT))
		m_pOwner->AddPosition(vRight * deltaTime);
}
