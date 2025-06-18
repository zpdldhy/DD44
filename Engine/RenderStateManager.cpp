#include "pch.h"
#include "RenderStateManager.h"
#include "Device.h"

void RenderStateManager::InitState()
{
    RenderOption defaultOption;
    defaultOption.blend = BlendType::AlphaBlend;
    defaultOption.depth = DepthType::ZTestOn_ZWriteOn;
    defaultOption.cull = CullType::Back;
    defaultOption.sampler = SamplerType::Linear;
    defaultOption.bWireframe = false;
    defaultOption.bStencil = false;

    m_curOption = defaultOption;
    m_prevOption = defaultOption;
    Apply(defaultOption);
}

void RenderStateManager::Apply(const RenderOption& option)
{
    m_optionStack.push(m_curOption);
    m_curOption = option;

    // --- Blend 설정 ---
    switch (option.blend)
    {
    case BlendType::Opaque:
        DC->OMSetBlendState(STATE->m_pOpaqueBlend.Get(), nullptr, 0xffffffff);
        break;
    case BlendType::AlphaBlend:
        DC->OMSetBlendState(STATE->m_pAlphaBlend.Get(), nullptr, 0xffffffff);
        break;
    case BlendType::Additive:
        DC->OMSetBlendState(STATE->m_pAdditiveBlend.Get(), nullptr, 0xffffffff);
        break;
    case BlendType::Multiply:
        DC->OMSetBlendState(STATE->m_pMultiplyBlend.Get(), nullptr, 0xffffffff);
        break;
    case BlendType::Premultiplied:
        DC->OMSetBlendState(STATE->m_pPremultipliedAlphaBlend.Get(), nullptr, 0xffffffff);
        break;
    case BlendType::DualSource:
        DC->OMSetBlendState(STATE->m_pDualSourceBlend.Get(), nullptr, 0xffffffff);
        break;
    }


    // --- Depth 설정 ---
    switch (option.depth)
    {
    case DepthType::ZTestOn_ZWriteOn:
        DC->OMSetDepthStencilState(STATE->m_pDSSDepthEnable.Get(), 0);
        break;
    case DepthType::ZTestOn_ZWriteOff:
        DC->OMSetDepthStencilState(STATE->m_pDSSDepthEnableZero.Get(), 0);
        break;
    case DepthType::ZTestOff:
        DC->OMSetDepthStencilState(STATE->m_pDSSDepthDisableZero.Get(), 0);
        break;
    }

    // --- Rasterizer 설정 ---
    if (option.bWireframe)
    {
        DC->RSSetState(STATE->m_pRSWireFrame.Get());
    }
    else
    {
        switch (option.cull)
        {
        case CullType::Back:
            DC->RSSetState(STATE->m_pRSSolid.Get()); break;
        case CullType::Front:
            // FrontCull 없으면 추가 필요
            break;
        case CullType::None:
            DC->RSSetState(STATE->m_pRSSolidNone.Get()); break;
        }
    }

    // Sampler (PS 0번 슬롯 기준)
    switch (option.sampler)
    {
    case SamplerType::Linear:
        DC->PSSetSamplers(0, 1, STATE->m_pLinearSS.GetAddressOf()); break;
    case SamplerType::Point:
        DC->PSSetSamplers(0, 1, STATE->m_pPointSS.GetAddressOf()); break;
    case SamplerType::Clamp:
        DC->PSSetSamplers(0, 1, STATE->m_pClampSS.GetAddressOf()); break;
    case SamplerType::Mirror:
        DC->PSSetSamplers(0, 1, STATE->m_pMirrorSS.GetAddressOf()); break;
    case SamplerType::Border:
        DC->PSSetSamplers(0, 1, STATE->m_pBorderSS.GetAddressOf()); break;
    }

    switch (option.rasterizer)
    {
    case RasterizerType::SolidBack:
        DC->RSSetState(STATE->m_pRSSolid.Get()); break;
    case RasterizerType::SolidNone:
        DC->RSSetState(STATE->m_pRSSolidNone.Get()); break;
    case RasterizerType::Wireframe:
        DC->RSSetState(STATE->m_pRSWireFrame.Get()); break;
    case RasterizerType::ShadowBias:
        DC->RSSetState(STATE->m_pRSShadowBias.Get()); break;
    }
}

void RenderStateManager::Restore()
{
    if (!m_optionStack.empty())
    {
        RenderOption prev = m_optionStack.top();
        m_optionStack.pop();
        m_curOption = prev;
        Apply(prev); // 실제 DX 적용
    }
}

