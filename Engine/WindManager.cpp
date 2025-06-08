#include "pch.h"
#include "WindManager.h"
#include "AWindActor.h"
#include "Device.h"
#include "RenderStateManager.h"

void WindManager::Init()
{
    // m_vWindList.clear();
    m_pWindTexture = std::make_unique<ViewPortTexture>();
    m_pWindTexture->CreateViewPortTexture(g_windowSize.x, g_windowSize.y); // 윈도우 크기 기준
}

void WindManager::Tick()
{
    for (auto it = m_vWindList.begin(); it != m_vWindList.end(); )
    {
        auto& wind = *it;
        if (wind->m_bDelete)
            it = m_vWindList.erase(it);
        else
        {
            wind->Tick();
            ++it;
        }
    }
}

void WindManager::Render()
{
    if (!m_pWindTexture)
        return;

    // ViewPortTexture에 바람 이펙트만 렌더링
    ID3D11RenderTargetView* prevRTV = nullptr;
    ID3D11DepthStencilView* prevDSV = nullptr;
    D3D11_VIEWPORT prevVP = {};
    UINT numVPs = 1;

    DC->RSGetViewports(&numVPs, &prevVP);
    DC->OMGetRenderTargets(1, &prevRTV, &prevDSV);

    ID3D11RenderTargetView* pRTV = m_pWindTexture->GetRTV();
    ID3D11DepthStencilView* pDSV = m_pWindTexture->GetDSV();
    DC->OMSetRenderTargets(1, &pRTV, pDSV);
    D3D11_VIEWPORT vp = m_pWindTexture->GetVP();
    DC->RSSetViewports(1, &vp);

    m_pWindTexture->ClearViewPortTransparent();

    RenderOption windOption;
    windOption.blend = BlendType::AlphaBlend;
    windOption.depth = DepthType::ZTestOn_ZWriteOff;     // 깊이 무시 (ZTestOff) 또는 ZWriteOff
    windOption.cull = CullType::None;             // 이펙트는 보통 Cull 없음
    windOption.sampler = SamplerType::Linear;

    STATEMANAGER->Apply(windOption);  //  Wind 렌더링 설정 적용

    for (auto& wind : m_vWindList)
        wind->Render();

    STATEMANAGER->Restore();          //  이전 상태로 복구

    // 이전 상태 복원
    DC->OMSetRenderTargets(1, &prevRTV, prevDSV);
    DC->RSSetViewports(1, &prevVP);

    if (prevRTV) prevRTV->Release();
    if (prevDSV) prevDSV->Release();
}

void WindManager::AddWind(std::shared_ptr<AWindActor> wind)
{
    m_vWindList.push_back(wind);
    wind->Init(); // 생성 후 Init 호출
}

ID3D11ShaderResourceView* WindManager::GetSRV() const
{
    return m_pWindTexture ? m_pWindTexture->GetSRV() : nullptr;
}



