#include "pch.h"
#include "GSParticleManager.h"
#include "UGSParticleComponent.h"

void GSParticleManager::Add(const shared_ptr<UGSParticleComponent>& component)
{
	m_vComponents.push_back(component);
}

void GSParticleManager::Render()
{
	for (auto& gs : m_vComponents)
		gs->Render();
}
