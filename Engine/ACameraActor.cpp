#include "pch.h"
#include "ACameraActor.h"

void ACameraActor::Init()
{
	m_pCameraComponent->Init();
	m_pCameraComponent->SetOwner(this);
}

void ACameraActor::Tick()
{
	AActor::Tick();
	m_pCameraComponent->Tick();
}

void ACameraActor::Render()
{
	m_pCameraComponent->Render();
}