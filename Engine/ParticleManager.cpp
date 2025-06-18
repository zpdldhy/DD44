#include "pch.h"
#include "ParticleManager.h"
#include "AParticleActor.h"
#include "CameraManager.h"
#include "AInstance.h"

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
	for (auto& pParticleActor : m_vInstanceList)
		pParticleActor->Render();

	m_vRenderParticleList.clear();
	PostRender();
}

void ParticleManager::PreRender()
{
	// 1. RenderTarget 저장
	ID3D11RenderTargetView* prevRTVs[D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT] = {};
	ID3D11DepthStencilView* prevDSV = nullptr;
	DC->OMGetRenderTargets(D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT, prevRTVs, &prevDSV);

	// 2. Attach → ref count 누적 방지
	for (UINT i = 0; i < D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT; ++i)
		m_pPrevRTVs[i].Attach(prevRTVs[i]); // AddRef 안 함
	m_pPrevDSV.Attach(prevDSV);

	// 3. RTV 하나만 설정 (Dual Source용 제한)
	ID3D11RenderTargetView* singleRTV[1] = { prevRTVs[0] };
	DC->OMSetRenderTargets(1, singleRTV, prevDSV);

	// 4. Dual Source 렌더 상태 적용
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
	// 1. 저장해둔 MRT/DSV 복구
	ID3D11RenderTargetView* restoreRTVs[D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT] = {};
	for (UINT i = 0; i < D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT; ++i)
		restoreRTVs[i] = m_pPrevRTVs[i].Get();

	DC->OMSetRenderTargets(D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT, restoreRTVs, m_pPrevDSV.Get());

	// 2. RenderState 복원
	STATEMANAGER->Restore();
}

void ParticleManager::Destroy()
{
	m_vParticleList.clear();
	m_vRenderParticleList.clear();
}

void ParticleManager::AddParticleList(vector<shared_ptr<AParticleActor>> _vParticleList)
{
	for (auto& pParticle : _vParticleList)
	{
		pParticle->Init();
		m_vParticleList.emplace_back(pParticle);
		SetInstance(pParticle);
	}
}

void ParticleManager::AddUI(shared_ptr<AParticleActor> _vParticleActor)
{
	_vParticleActor->Init();
	m_vParticleList.emplace_back(_vParticleActor);

	SetInstance(_vParticleActor);
}

void ParticleManager::SetInstance(shared_ptr<AParticleActor> _pActor)
{
	auto meshCom = _pActor->GetMeshComponent();

	for (auto& pInstance : m_vInstanceList)
	{
		// Mesh가 다르면 넘어가요
		if (pInstance->GetMeshPath() != meshCom->GetMeshPath())
			continue;

		// Texture만 다르면 생성
		if (pInstance->GetTexturePath() != meshCom->GetMaterial()->GetTexturePath())
		{
			MakeInstance(_pActor);
			return;
		}
		else
		{
			pInstance->AddInstanceMesh(meshCom);
			return;
		}
	}

	// 다 찾아보고 없으면 생성
	MakeInstance(_pActor);
}

void ParticleManager::MakeInstance(shared_ptr<AParticleActor> _pActor)
{
	auto pInstance = make_shared<AInstance>();

	pInstance->SetInstanceMesh(_pActor->GetMeshComponent());
	pInstance->m_bUseStencil = _pActor->m_bUseStencil;
	pInstance->Init();

	m_vInstanceList.emplace_back(pInstance);
}
