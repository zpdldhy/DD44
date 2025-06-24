#include "pch.h"
#include "BloomManager.h"
#include "Device.h"
#include "RenderStateManager.h"

void BloomManager::Init()
{
	m_pDownScaleRT = std::make_unique<ViewPortTexture>();
	m_pDownScaleRT->CreateViewPortTexture(g_windowSize.x / 2, g_windowSize.y / 2);
}

void BloomManager::BeginBloomPass()
{
	//현재 상태 저장
	UINT numVPs = 1;
	DC->RSGetViewports(&numVPs, &m_tPrevVP);
	m_iPrevRTVCount = 8;
	DC->OMGetRenderTargets(m_iPrevRTVCount, m_arrPrevRTV[0].GetAddressOf(), m_pPrevDSV.GetAddressOf());


	// 다운스케일 RT 바인딩
	ID3D11RenderTargetView* rtv = m_pDownScaleRT->GetRTV();
	DC->OMSetRenderTargets(1, &rtv, m_pDownScaleRT->GetDSV());
	DC->RSSetViewports(1, &m_pDownScaleRT->GetVP());
	m_pDownScaleRT->ClearViewPortTransparent();

	RenderOption opt;
	opt.blend = BlendType::BloomStrongColor;
	opt.depth = DepthType::ZTestOff;
	opt.cull = CullType::None;
	opt.sampler = SamplerType::Linear;

	STATEMANAGER->Apply(opt);

}

void BloomManager::EndBloomPass()
{
	STATEMANAGER->Restore();

	ID3D11RenderTargetView* rtvs[8] = {};
	for (UINT i = 0; i < m_iPrevRTVCount; ++i)
		rtvs[i] = m_arrPrevRTV[i].Get();

	DC->OMSetRenderTargets(m_iPrevRTVCount, rtvs, m_pPrevDSV.Get());
	DC->RSSetViewports(1, &m_tPrevVP);

	for (UINT i = 0; i < m_iPrevRTVCount; ++i)
	m_arrPrevRTV[i].Reset();
	m_pPrevDSV.Reset();
}



