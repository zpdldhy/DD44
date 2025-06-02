#include "pch.h"
#include "AActor.h"
#include "Device.h"
#include "UScriptComponent.h"
#include "ObjectManager.h"

AActor::AActor()
{
	m_pTransform = make_unique<USceneComponent>();
	m_pPhysics = make_unique<UPhysicsComponent>();
}

void AActor::Init()
{
	m_pTransform->SetOwner(shared_from_this());
	m_pTransform->Init();

	m_pPhysics->SetOwner(shared_from_this());
	m_pPhysics->Init();

	for (auto& component : m_arrComponent)
	{
		if (component)
		{
			component->SetOwner(shared_from_this());
			component->Init();
		}
	}

	for (auto& script : m_vScript)
	{
		script->SetOwner(shared_from_this());
		script->Init();
	}	
}

void AActor::Tick()
{
	m_pPhysics->Tick();

	m_pTransform->Tick();

	// Component
	for (auto& component : m_arrComponent)
	{
		if (component)
		{
			component->Tick();
		}
	}

	for (auto& script : m_vScript)
	{
		script->Tick();
	}

	if (GetShapeComponent())
	{
		GetShapeComponent()->ResetCollisionList();
	}
}

void AActor::Render()
{
	if (!m_bRender)
		return;

	m_pTransform->Render();
	m_pPhysics->Render();

	for (auto& component : m_arrComponent)
	{
		if (component)
		{
			component->Render();
		}
	}

	for (auto& script : m_vScript)
	{
		script->Render();
	}
}

void AActor::Destroy()
{
	m_pTransform->Destroy();
	m_pPhysics->Destroy();

	for (auto& component : m_arrComponent)
	{
		if (component)
		{
			component->Destroy();
		}
	}

	for (auto& script : m_vScript)
	{
		script->Destroy();
	}
}