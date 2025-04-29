#include "pch.h"
#include "EngineCameraMoveScript.h"
#include "Input.h"
#include "Timer.h"

void EngineCameraMoveScript::Tick()
{
	float deltaTime = TIMER->GetDeltaTime() * 10.f;

	if (INPUT->GetButtonDown(W))
		m_pOwner->AddPosition(-m_pOwner->GetLook() * deltaTime);

	if (INPUT->GetButtonDown(A))
		m_pOwner->AddPosition(-m_pOwner->GetRight() * deltaTime);

	if (INPUT->GetButtonDown(S))
		m_pOwner->AddPosition(m_pOwner->GetLook() * deltaTime);

	if (INPUT->GetButtonDown(D))
		m_pOwner->AddPosition(m_pOwner->GetRight() * deltaTime);

	if (INPUT->GetButtonDown(Q))
		m_pOwner->AddPosition(Vec3(0.0f, -1.0f, 0.0f) * deltaTime);

	if (INPUT->GetButtonDown(E))
		m_pOwner->AddPosition(Vec3(0.0f, 1.0f, 0.0f) * deltaTime);

	if (INPUT->GetButton(RCLICK))
		m_pPrevMousePos = INPUT->GetMousePos();

	if (INPUT->GetButtonDown(RCLICK))
	{
		POINT pCurrentMousePointer = INPUT->GetMousePos();
		float fYaw = (m_pPrevMousePos.x - pCurrentMousePointer.x) * deltaTime / 4.f;
		float fPitch = (m_pPrevMousePos.y - pCurrentMousePointer.y) * deltaTime / 4.f;

		m_pOwner->AddRotation(Vec3(fPitch, fYaw, 0.0f));

		m_pPrevMousePos = pCurrentMousePointer;
	}
}