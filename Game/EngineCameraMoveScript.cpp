#include "pch.h"
#include "EngineCameraMoveScript.h"
#include "Input.h"
#include "Timer.h"

void EngineCameraMoveScript::Tick()
{
	float deltaTime = TIMER->GetDeltaTime() * 10.f;

	auto vLook = m_pOwner->GetLook();
	auto vRight = m_pOwner->GetRight();

	if (INPUT->GetButtonDown(W))
		m_pOwner->AddPosition(vLook * deltaTime);

	if (INPUT->GetButtonDown(A))
		m_pOwner->AddPosition(-vRight * deltaTime);

	if (INPUT->GetButtonDown(S))
		m_pOwner->AddPosition(-vLook * deltaTime);

	if (INPUT->GetButtonDown(D))
		m_pOwner->AddPosition(vRight * deltaTime);

	if (INPUT->GetButtonDown(Q))
		m_pOwner->AddPosition(Vec3(0.0f, -1.0f, 0.0f) * deltaTime);

	if (INPUT->GetButtonDown(E))
		m_pOwner->AddPosition(Vec3(0.0f, 1.0f, 0.0f) * deltaTime);

	if (INPUT->GetButton(RCLICK))
		m_pPrevMousePos = INPUT->GetMousePos();

	if (INPUT->GetButtonDown(RCLICK))
	{
		POINT pCurrentMousePointer = INPUT->GetMousePos();
		float fYaw = (pCurrentMousePointer.x - m_pPrevMousePos.x) * deltaTime / 4.f;
		float fPitch = (pCurrentMousePointer.y - m_pPrevMousePos.y) * deltaTime / 4.f;

		m_pOwner->AddRotation(Vec3(fPitch, fYaw, 0.0f));

		m_pPrevMousePos = pCurrentMousePointer;
	}
}