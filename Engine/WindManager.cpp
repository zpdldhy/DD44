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
    m_vWindList.clear();

    m_sharedOption.blend = BlendType::AlphaBlend;
    m_sharedOption.depth = DepthType::ZTestOff;
    m_sharedOption.cull = CullType::None;
    m_sharedOption.sampler = SamplerType::Linear;
    m_sharedOption.bWireframe = false;
    m_sharedOption.bStencil = false;

    m_pWindTarget = make_unique<ViewPortTexture>();
    m_pWindTarget->CreateViewPortTexture((float)g_windowSize.x, (float)g_windowSize.y);

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

    FLOAT clearColor[4] = { 1, 0, 0, 1.f };
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
    OutputDebugStringA(" Wind Added!\n");
}

ID3D11ShaderResourceView* WindManager::GetSRV()
{
    return m_pWindTarget->GetSRV();
}