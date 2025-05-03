#include "pch.h"
#include "AActor.h"
#include "Device.h"
#include "UScriptComponent.h"

AActor::AActor()
{
	m_pTransform = make_shared<USceneComponent>();
	m_arrComponent[static_cast<size_t>(ComponentType::CT_TRANSFORM)] = m_pTransform;
}

void AActor::Init()
{
	for (auto& component : m_arrComponent)
	{
		if (component)
		{
			component->SetOwner(this);
			component->Init();
		}
	}

	for (auto& script : m_vScript)
	{
		script->SetOwner(this);
		script->Init();
	}
}

void AActor::Tick()
{
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
}

void AActor::Render()
{
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