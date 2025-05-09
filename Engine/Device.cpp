#include "pch.h"
#include "Device.h"
#include "DxState.h"
#include "Input.h"

void Device::CreateDeviceAndSwapChain()
{
    D3D_FEATURE_LEVEL pFeatureLevel;
    IDXGIAdapter* pAdapter = nullptr;
    D3D_DRIVER_TYPE DriverType = D3D_DRIVER_TYPE_HARDWARE;
    HMODULE Software = NULL;
    UINT Flags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#ifdef _DEBUG
    Flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    D3D_FEATURE_LEVEL pFeatureLevels = D3D_FEATURE_LEVEL_11_0;
    UINT FeatureLevels = 1;
    UINT SDKVersion = D3D11_SDK_VERSION;

    DXGI_SWAP_CHAIN_DESC scd;
    ZeroMemory(&scd, sizeof(scd));
    scd.BufferDesc.Width = g_windowSize.x;
    scd.BufferDesc.Height = g_windowSize.y;
    scd.BufferDesc.RefreshRate.Numerator = 60;
    scd.BufferDesc.RefreshRate.Denominator = 1;
    scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    scd.BufferCount = 2;                                   // 트리플 버퍼링
    scd.OutputWindow = g_hWnd;
    scd.Windowed = true;
    scd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;        // Win8 이상 호환성 높임
    scd.SampleDesc.Count = 1;

    HRESULT hr = D3D11CreateDeviceAndSwapChain(
        pAdapter,
        DriverType,
        Software,
        Flags,
        &pFeatureLevels,
        FeatureLevels,
        SDKVersion,
        &scd,
        m_pSwapChain.GetAddressOf(),
        m_pd3dDevice.GetAddressOf(),
        &pFeatureLevel,
        m_pd3dContext.GetAddressOf());

    if (FAILED(hr))
    {
        DX_CHECK(hr, _T("CreateDeviceAndSwapChain"));
    }
}

void Device::CreateRenderTargetView()
{
    ComPtr<ID3D11Texture2D> pBackBuffer;
    HRESULT hr = m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);

    if (FAILED(hr))
    {
        DX_CHECK(hr, _T(__FUNCTION__));
    }

    hr = m_pd3dDevice->CreateRenderTargetView(pBackBuffer.Get(), NULL, m_pRTV.GetAddressOf());

    if (FAILED(hr))
    {
        DX_CHECK(hr, _T(__FUNCTION__));
    }
}

void Device::CreateDepthStencilBuffer()
{
    D3D11_TEXTURE2D_DESC desc;
    ZeroMemory(&desc, sizeof(desc));

    desc.Width = g_windowSize.x;
    desc.Height = g_windowSize.y;
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    desc.SampleDesc.Count = 1;
    desc.SampleDesc.Quality = 0;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    desc.CPUAccessFlags = 0;
    desc.MiscFlags = 0;

    HRESULT hr = m_pd3dDevice->CreateTexture2D(
        &desc, NULL, m_pDepthStencilTexture.GetAddressOf());

    if (FAILED(hr))
    {
        DX_CHECK(hr, _T(__FUNCTION__));
    }

    D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
    ZeroMemory(&dsvDesc, sizeof(dsvDesc));

    dsvDesc.Format = desc.Format;
    dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;

    hr = m_pd3dDevice->CreateDepthStencilView(
        m_pDepthStencilTexture.Get(), &dsvDesc, m_pDSV.GetAddressOf());

    if (FAILED(hr))
    {
        DX_CHECK(hr, _T(__FUNCTION__));
    }
}

void Device::SetViewport()
{
    m_MainVP.Width = (FLOAT)g_windowSize.x;
    m_MainVP.Height = (FLOAT)g_windowSize.y;
    m_MainVP.MinDepth = 0.0f;
    m_MainVP.MaxDepth = 1.0f;
    m_MainVP.TopLeftX = 0.0f;
    m_MainVP.TopLeftY = 0.0f;
}

void Device::SetDefaultState()
{
    m_pd3dContext->PSSetSamplers(0, 1, STATE->m_pLinearSS.GetAddressOf());
    m_pd3dContext->OMSetBlendState(STATE->m_pAlphaBlend.Get(), 0, -1);

    if (!m_bWireFrame)
    {
        m_pd3dContext->RSSetState(STATE->m_pRSSolid.Get());
    }
    else
    {
        m_pd3dContext->RSSetState(STATE->m_pRSWireFrame.Get());
    }

    m_pd3dContext->OMSetDepthStencilState(STATE->m_pDSSDepthEnable.Get(), 0);
}

void Device::CreateBloomRenderTarget()
{
    D3D11_TEXTURE2D_DESC desc = {};
    desc.Width = g_windowSize.x;
    desc.Height = g_windowSize.y;
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    desc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT; // Bloom용 고정밀
    desc.SampleDesc.Count = 1;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

    HRESULT hr = m_pd3dDevice->CreateTexture2D(&desc, nullptr, m_pBloomTexture.GetAddressOf());
    DX_CHECK(hr, _T("CreateBloomRenderTarget - Texture"));

    hr = m_pd3dDevice->CreateRenderTargetView(m_pBloomTexture.Get(), nullptr, m_pBloomRTV.GetAddressOf());
    DX_CHECK(hr, _T("CreateBloomRenderTarget - RTV"));

    hr = m_pd3dDevice->CreateShaderResourceView(m_pBloomTexture.Get(), nullptr, m_pBloomSRV.GetAddressOf());
    DX_CHECK(hr, _T("CreateBloomRenderTarget - SRV"));
}

void Device::OnResize(UINT _width, UINT _height)
{
    if (!m_pSwapChain) return;

    if (m_pd3dContext)
    {
        m_pd3dContext->OMSetRenderTargets(0, nullptr, nullptr);
    }

    // RTV, DSV 등 리소스 해제
    m_pRTV.Reset();
    m_pDSV.Reset();
    m_pDepthStencilTexture.Reset();

    // 스왑 체인 크기 변경
    HRESULT hr = m_pSwapChain->ResizeBuffers(0, _width, _height, DXGI_FORMAT_UNKNOWN, 0);
    if (FAILED(hr))
    {
        DX_CHECK(hr, _T("ResizeBuffers"));
        return;
    }

    // 최신 크기 갱신
    g_windowSize.x = _width;
    g_windowSize.y = _height;

    // 다시 생성
    CreateRenderTargetView();
    CreateDepthStencilBuffer();
    SetViewport();
}

void Device::Init()
{
    CreateDeviceAndSwapChain();
    CreateRenderTargetView();
    CreateDepthStencilBuffer();
    CreateBloomRenderTarget();
    SetViewport();
}

void Device::Frame()
{
    if (INPUT->GetButton(V))
    {
        m_bWireFrame = !m_bWireFrame;
    }
}

void Device::PreRender()
{
    m_pd3dContext->RSSetViewports(1, &m_MainVP);

    //todo bloom 분기 조절하기.
    bool bUseBloom = true;

    if (bUseBloom)
    {
        ID3D11RenderTargetView* rtvs[2] = { m_pRTV.Get(), m_pBloomRTV.Get() };
        m_pd3dContext->OMSetRenderTargets(2, rtvs, m_pDSV.Get());

        float black[4] = { 0, 0, 0, 1 };
        m_pd3dContext->ClearRenderTargetView(m_pBloomRTV.Get(), black);
    }
    else
    {
        m_pd3dContext->OMSetRenderTargets(1, m_pRTV.GetAddressOf(), m_pDSV.Get());
    }

    float clearColor[4] = { 0.1f, 0.25f, 0.4f, 1.0f };
    m_pd3dContext->ClearRenderTargetView(m_pRTV.Get(), clearColor);
    m_pd3dContext->ClearDepthStencilView(m_pDSV.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1, 0);

    m_pd3dContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    SetDefaultState();
}

void Device::Render()
{
    PreRender();
    PostRender();
}

void Device::PostRender()
{
    m_pSwapChain->Present(0, 0);
}

void Device::Release()
{

}

ComPtr<ID3D11Device> Device::GetDevice()
{
    return m_pd3dDevice;
}

ComPtr<IDXGISwapChain> Device::GetSwapChain()
{
    return m_pSwapChain;
}

ComPtr<ID3D11DeviceContext> Device::GetDeviceContext()
{
    return m_pd3dContext;
}
