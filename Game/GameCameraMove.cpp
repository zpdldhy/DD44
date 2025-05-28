#include "pch.h"
#include "GameCameraMove.h"

void GameCameraMove::Init()
{
	m_vOffset = { 20.0f, 30.0f, -20.0f };
}

void GameCameraMove::Tick()
{
	auto camera = GetOwner()->GetCameraComponent();
	camera->SetLookAt(m_pTarget->GetPosition());

	//Set Position
	GetOwner()->SetPosition(m_pTarget->GetPosition() + m_vOffset);
}
