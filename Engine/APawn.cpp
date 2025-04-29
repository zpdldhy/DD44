#include "pch.h"
#include "APawn.h"

void APawn::Init()
{
	AActor::Init();
}

void APawn::Tick()
{
	AActor::Tick();
}

void APawn::Render()
{
	m_Components[ComponentType::CT_MESH]->PreRender();

	AActor::Render();

	m_Components[ComponentType::CT_MESH]->PostRender();
}

void APawn::Destroy()
{
	AActor::Destroy();
}
