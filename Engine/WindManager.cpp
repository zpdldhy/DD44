#include "pch.h"
#include "WindManager.h"
#include "AWindActor.h"
#include "Device.h"
#include "RenderStateManager.h"

void WindManager::Init()
{
    // m_vWindList.clear();
    m_pWindTexture = std::make_unique<ViewPortTexture>();
    m_pWindTexture->CreateViewPortTexture(g_windowSize.x, g_windowSize.y); // ������ ũ�� ����
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

    // ViewPortTexture�� �ٶ� ����Ʈ�� ������
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
    windOption.depth = DepthType::ZTestOn_ZWriteOff;     // ���� ���� (ZTestOff) �Ǵ� ZWriteOff
    windOption.cull = CullType::None;             // ����Ʈ�� ���� Cull ����
    windOption.sampler = SamplerType::Linear;

    STATEMANAGER->Apply(windOption);  //  Wind ������ ���� ����

    for (auto& wind : m_vWindList)
        wind->Render();

    STATEMANAGER->Restore();          //  ���� ���·� ����

    // ���� ���� ����
    DC->OMSetRenderTargets(1, &prevRTV, prevDSV);
    DC->RSSetViewports(1, &prevVP);

    if (prevRTV) prevRTV->Release();
    if (prevDSV) prevDSV->Release();
}

void WindManager::AddWind(std::shared_ptr<AWindActor> wind)
{
    m_vWindList.push_back(wind);
    wind->Init(); // ���� �� Init ȣ��
}

ID3D11ShaderResourceView* WindManager::GetSRV() const
{
    return m_pWindTexture ? m_pWindTexture->GetSRV() : nullptr;
}



