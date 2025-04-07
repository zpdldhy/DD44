#include "pch.h"
#include "Device.h"
#include "DxState.h"

bool Device::CreateDevice()
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
        _pSwapChain.GetAddressOf(),
        _pd3dDevice.GetAddressOf(),
        &pFeatureLevel,
        &_pd3dContext);

    if (FAILED(hr))
    {
        DX_CHECK(hr, _T(__FUNCTION__));
        return false;
    }

    ID3D11Texture2D* pBackBuffer;
    hr = _pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
    if (FAILED(hr))
    {
        DX_CHECK(hr, _T(__FUNCTION__));
        return false;
    }
    hr = _pd3dDevice->CreateRenderTargetView(pBackBuffer, NULL, &_pRTV);
    pBackBuffer->Release();
    if (FAILED(hr))
    {
        DX_CHECK(hr, _T(__FUNCTION__));
        return false;
    }
    // viewport
    _MainVP.Width = (FLOAT)g_windowSize.x;
    _MainVP.Height = (FLOAT)g_windowSize.y;
    _MainVP.MinDepth = 0.0f;
    _MainVP.MaxDepth = 1.0f;
    _MainVP.TopLeftX = 0;
    _MainVP.TopLeftY = 0;

    CreateDepthStencilBuffer();

    _pd3dContext->RSSetViewports(1, &_MainVP);
    _pd3dContext->OMSetRenderTargets(1, _pRTV.GetAddressOf(), _pDSV.Get());

    return true;
}
bool Device::CreateDepthStencilBuffer()
{
    HRESULT hr;
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
    hr = _pd3dDevice->CreateTexture2D(&desc, NULL,
        _pDepthStencilTexture.GetAddressOf());
    if (FAILED(hr))
    {
        DX_CHECK(hr, _T("CreateTexture2D"));
        return false;
    }

    D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
    ZeroMemory(&dsvDesc, sizeof(dsvDesc));
    dsvDesc.Format = desc.Format;
    dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;

    if (FAILED(hr = _pd3dDevice->CreateDepthStencilView(
        _pDepthStencilTexture.Get(), &dsvDesc,
        _pDSV.GetAddressOf())))
    {
        DX_CHECK(hr, _T("CreateDepthStencilView"));
        return false;
    }
    return true;
}

void Device::Init()
{
    if (!CreateDevice())
    {

    }
}
void Device::Frame()
{

}
void Device::PreRender()
{
    float ClearColor[] = { 0.1f, 0.25f, 0.4f, 1.0f };
    _pd3dContext->ClearRenderTargetView(_pRTV.Get(), ClearColor);
    // DEPTH STENCIL VIEW ÃÊ±âÈ­
    _pd3dContext->ClearDepthStencilView(_pDSV.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1, 0);
    _pd3dContext->PSSetSamplers(0, 1, STATE->_linearSS.GetAddressOf());
    //_pd3dContext->OMSetBlendState(TDxState::GetInstance().m_pAlphaBlend.Get(), 0, -1);
}
void Device::Render()
{
    PreRender();
    PostRender();
}
void Device::PostRender()
{
    _pSwapChain->Present(0, 0);
}
void Device::Release()
{
}

ComPtr<ID3D11Device> Device::GetDevice()
{
    return _pd3dDevice;
}
ComPtr<IDXGISwapChain> Device::GetSwapChain()
{
    return _pSwapChain;
}
ComPtr<ID3D11DeviceContext> Device::GetDeviceContext()
{
    return _pd3dContext;
}
