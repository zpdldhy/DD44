#include "pch.h"
#include "EngineCameraMoveScript.h"
#include "Input.h"
#include "Timer.h"
#include "CollisionManager.h"
#include "AActor.h"

void EngineCameraMoveScript::Init()
{
	auto camera = GetOwner()->GetCameraComponent();
	camera->SetLookTo(GetOwner()->GetLook());
}

void EngineCameraMoveScript::Tick()
{
	float deltaTime = TIMER->GetDeltaTime() * 10.f;

	if (INPUT->GetButton(RCLICK))
		m_pPrevMousePos = INPUT->GetMousePos();

	if (INPUT->GetButtonDown(RCLICK))
	{
		POINT pCurrentMousePointer = INPUT->GetMousePos();
		float fYaw = (pCurrentMousePointer.x - m_pPrevMousePos.x) * deltaTime / 20.f;
		float fPitch = (pCurrentMousePointer.y - m_pPrevMousePos.y) * deltaTime / 20.f;

		GetOwner()->AddRotation(Vec3(fPitch, fYaw, 0.0f));

		m_pPrevMousePos = pCurrentMousePointer;
	}

	if(INPUT->GetButtonDown(LSHIFT))
		deltaTime *= 10.f;

	if (INPUT->GetButtonDown(W))
		GetOwner()->AddPosition(GetOwner()->GetLook() * deltaTime);

	if (INPUT->GetButtonDown(A))
		GetOwner()->AddPosition(-GetOwner()->GetRight() * deltaTime);

	if (INPUT->GetButtonDown(S))
		GetOwner()->AddPosition(-GetOwner()->GetLook() * deltaTime);

	if (INPUT->GetButtonDown(D))
		GetOwner()->AddPosition(GetOwner()->GetRight() * deltaTime);

	if (INPUT->GetButtonDown(Q))
		GetOwner()->AddPosition(Vec3(0.0f, -1.0f, 0.0f) * deltaTime);

	if (INPUT->GetButtonDown(E))
		GetOwner()->AddPosition(Vec3(0.0f, 1.0f, 0.0f) * deltaTime);

	auto camera = GetOwner()->GetCameraComponent();
	camera->SetLookTo(GetOwner()->GetLook());
}