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
	// 1. RenderTarget ����
	ID3D11RenderTargetView* prevRTVs[D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT] = {};
	ID3D11DepthStencilView* prevDSV = nullptr;
	DC->OMGetRenderTargets(D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT, prevRTVs, &prevDSV);

	// 2. Attach �� ref count ���� ����
	for (UINT i = 0; i < D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT; ++i)
		m_pPrevRTVs[i].Attach(prevRTVs[i]); // AddRef �� ��
	m_pPrevDSV.Attach(prevDSV);

	// 3. RTV �ϳ��� ���� (Dual Source�� ����)
	ID3D11RenderTargetView* singleRTV[1] = { prevRTVs[0] };
	DC->OMSetRenderTargets(1, singleRTV, prevDSV);

	// 4. Dual Source ���� ���� ����
	m_sharedOption.blend = BlendType::DualSource;
	m_sharedOption.depth = DepthType::ZTestOn_ZWriteOff;
	m_sharedOption.cull = CullType::None;
	m_sharedOption.sampler = SamplerType::Linear;
	m_sharedOption.bWireframe = false;
	m_sharedOption.bStencil = false;

	STATEMANAGER->Apply(m_sharedOption);
}

void ParticleManager::PostRender()
{
	// 1. �����ص� MRT/DSV ����
	ID3D11RenderTargetView* restoreRTVs[D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT] = {};
	for (UINT i = 0; i < D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT; ++i)
		restoreRTVs[i] = m_pPrevRTVs[i].Get();

	DC->OMSetRenderTargets(D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT, restoreRTVs, m_pPrevDSV.Get());

	// 2. RenderState ����
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
