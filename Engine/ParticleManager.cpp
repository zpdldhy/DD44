#include "pch.h"
#include "ParticleManager.h"
#include "AParticleActor.h"
#include "CameraManager.h"


void ParticleManager::Tick()
{
	for (auto pP = m_vParticleList.begin(); pP != m_vParticleList.end();)
	{
		if (pP->get()->m_bDelete == true)
			pP = m_vParticleList.erase(pP);
		else
		{
			pP->get()->Tick();
			pP++;
		}
	}

	m_vRenderParticleList = m_vParticleList;
}

void ParticleManager::Render()
{
	PreRender();
	for (auto& pParticleActor : m_vRenderParticleList)
		pParticleActor->Render();

	m_vRenderParticleList.clear();
	PostRender();
}

void ParticleManager::PreRender()
{
	m_sharedOption.blend = BlendType::Additive;
	m_sharedOption.depth = DepthType::ZTestOn_ZWriteOff;
	m_sharedOption.cull = CullType::None;
	m_sharedOption.sampler = SamplerType::Linear;
	m_sharedOption.bWireframe = false;
	m_sharedOption.bStencil = false;

	STATEMANAGER->Apply(m_sharedOption);
}

void ParticleManager::PostRender()
{
	STATEMANAGER->Restore();
}

void ParticleManager::Destroy()
{
	m_vParticleList.clear();
	m_vRenderParticleList.clear();
}

void ParticleManager::SetParticleList(vector<shared_ptr<AParticleActor>> _vParticleList)
{
	for (auto& pParticle : _vParticleList)
		pParticle->Init();
	m_vParticleList = _vParticleList;
}

void ParticleManager::AddParticleList(vector<shared_ptr<AParticleActor>> _vParticleList)
{
	for (auto& pParticle : _vParticleList)
	{
		pParticle->Init();
		m_vParticleList.emplace_back(pParticle);
	}
		
	
}

void ParticleManager::AddUI(shared_ptr<AParticleActor> _vParticleActor)
{
	_vParticleActor->Init();
	m_vParticleList.emplace_back(_vParticleActor);
}
