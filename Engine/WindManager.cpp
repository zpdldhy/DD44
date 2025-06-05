#include "pch.h"
#include "WindManager.h"
#include "AWindActor.h"
#include "ViewPortTexture.h"
#include "Device.h"
#include "Timer.h"
#include "AWindQuadActor.h"
#include "DxState.h"

void WindManager::Init()
{
    m_sharedOption.blend = BlendType::AlphaBlend;
    m_sharedOption.depth = DepthType::ZTestOff;
    m_sharedOption.cull = CullType::None;
    m_sharedOption.sampler = SamplerType::Linear;
    m_sharedOption.bWireframe = false;
    m_sharedOption.bStencil = false;

    m_pWindTarget = make_unique<ViewPortTexture>();
    m_pWindTarget->CreateViewPortTexture((float)g_windowSize.x, (float)g_windowSize.y);

    m_pWindCompositeTarget = make_unique<ViewPortTexture>();
    m_pWindCompositeTarget->CreateViewPortTexture((float)g_windowSize.x, (float)g_windowSize.y);

    m_pQuadActor = make_shared<AWindQuadActor>();
    m_pQuadActor->Init();
    m_pQuadActor->SetSRV(m_pWindTarget->GetSRV());
}

void WindManager::Tick()
{
    for (auto& wind : m_vWindList)
        wind->Tick();
}

void WindManager::Render()
{


    PreRender();
    for (auto& wind : m_vWindList)
    {
        wind->Render();
    }
    PostRender();

    /*if (m_pQuadActor)
        m_pQuadActor->Render();*/
}

void WindManager::Destroy()
{
    m_vWindList.clear();
    m_pWindTarget.reset();
    m_pPrevRTV.Reset();
    m_pQuadActor.reset();
}

void WindManager::Present()
{
    if (m_pQuadActor)
        m_pQuadActor->Render();
}

void WindManager::PreRender()
{
    DC->OMGetRenderTargets(1, m_pPrevRTV.GetAddressOf(), nullptr);

    ID3D11RenderTargetView* rtv = m_pWindTarget->GetRTV();
    DC->OMSetRenderTargets(1, &rtv, nullptr);

    D3D11_VIEWPORT vp = m_pWindTarget->GetVP();
    DC->RSSetViewports(1, &vp);

    FLOAT clearColor[4] = { 1, 0, 0, 0.2f };
    DC->ClearRenderTargetView(rtv, clearColor);

    STATEMANAGER->Apply(m_sharedOption);

}

void WindManager::PostRender()
{
    DC->OMSetRenderTargets(1, m_pPrevRTV.GetAddressOf(), nullptr);
    m_pPrevRTV.Reset();



    STATEMANAGER->Restore();
}

void WindManager::AddWind(shared_ptr<AWindActor> wind)
{
    wind->Init();
    m_vWindList.push_back(wind);
}

ID3D11ShaderResourceView* WindManager::GetSRV()
{
    return m_pWindTarget->GetSRV();
}

void WindManager::CompositeQuadToTexture()
{
    ID3D11RenderTargetView* prevRTV = nullptr;
    DC->OMGetRenderTargets(1, &prevRTV, nullptr);

    auto rtv = m_pWindCompositeTarget->GetRTV();
    DC->OMSetRenderTargets(1, &rtv, nullptr);

    D3D11_VIEWPORT vp = m_pWindCompositeTarget->GetVP();
    DC->RSSetViewports(1, &vp);

    FLOAT clearColor[4] = { 0, 0, 0, 0 };
    DC->ClearRenderTargetView(rtv, clearColor);

    if (m_pQuadActor)
        m_pQuadActor->Render();

    DC->OMSetRenderTargets(1, &prevRTV, nullptr);
    if (prevRTV) prevRTV->Release();
}

ID3D11ShaderResourceView* WindManager::GetCompositeSRV()
{
    return m_pWindCompositeTarget->GetSRV();
}