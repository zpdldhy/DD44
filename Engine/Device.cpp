#include "pch.h"
#include "Device.h"
#include "DxState.h"

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
    scd.BufferCount = 1;
    scd.BufferDesc.Width = g_windowSize.x;
    scd.BufferDesc.Height = g_windowSize.y;
    scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    scd.OutputWindow = g_hWnd;
    scd.Windowed = true;
    scd.BufferDesc.RefreshRate.Numerator = 60;
    scd.BufferDesc.RefreshRate.Denominator = 1;
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
        &m_pd3dContext);

    if (FAILED(hr))
    {
        DX_CHECK(hr, _T(__FUNCTION__));
    }
}

void Device::CreateRenderTargetView()
{
    ID3D11Texture2D* pBackBuffer = nullptr;
    HRESULT hr = m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);

    if (FAILED(hr))
    {
        DX_CHECK(hr, _T(__FUNCTION__));
    }

    hr = m_pd3dDevice->CreateRenderTargetView(pBackBuffer, NULL, m_pRTV.GetAddressOf());
    pBackBuffer->Release();

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

void Device::Init()
{
    CreateDeviceAndSwapChain();
    CreateRenderTargetView();
    CreateDepthStencilBuffer();
    SetViewport();
}

void Device::Frame()
{

}

void Device::PreRender()
{
    m_pd3dContext->RSSetViewports(1, &m_MainVP);
    m_pd3dContext->OMSetRenderTargets(1, m_pRTV.GetAddressOf(), m_pDSV.Get());

    float ClearColor[] = { 0.1f, 0.25f, 0.4f, 1.0f };

    m_pd3dContext->ClearRenderTargetView(m_pRTV.Get(), ClearColor);
    m_pd3dContext->ClearDepthStencilView(m_pDSV.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1, 0);
    
    m_pd3dContext->PSSetSamplers(0, 1, STATE->_linearSS.GetAddressOf());
    //_pd3dContext->OMSetBlendState(TDxState::GetInstance().m_pAlphaBlend.Get(), 0, -1);
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
