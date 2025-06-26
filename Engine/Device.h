#pragma once
#include "Singleton.h"

class Device : public Singleton<Device>
{
    // Device & SwapChain
    ComPtr<ID3D11Device> m_pd3dDevice = nullptr;
    ComPtr<ID3D11DeviceContext> m_pd3dContext = nullptr;
    ComPtr<IDXGISwapChain> m_pSwapChain = nullptr;

    // Render Target View
    ComPtr<ID3D11RenderTargetView> m_pRTV = nullptr;

    // Depth Stencil View
    ComPtr<ID3D11DepthStencilView> m_pDSV = nullptr;
    ComPtr<ID3D11Texture2D> m_pDepthStencilTexture = nullptr;

    // Viewport
    D3D11_VIEWPORT m_MainVP = D3D11_VIEWPORT();

    // DxState
    bool m_bWireFrame = false;

private:
    void CreateDeviceAndSwapChain();
    void CreateRenderTargetView();
    void CreateDepthStencilBuffer();
    void SetViewport();
    void SetDefaultState();

public:
    void OnResize(UINT _width, UINT _height);
    void ChangeWireFrame() { m_bWireFrame = !m_bWireFrame; }

public:
    void Init();
    void Frame();
    void PreRender();
    void Render();
    void PostRender();
    void Release();

public:
    ComPtr<ID3D11Device> GetDevice();
    ComPtr< IDXGISwapChain> GetSwapChain();
    ComPtr<ID3D11DeviceContext> GetDeviceContext();
public:
    ID3D11RenderTargetView* GetBackBufferRTV() { return m_pRTV.Get(); }
    ID3D11DepthStencilView* GetDepthStencilView() { return m_pDSV.Get(); }
    D3D11_VIEWPORT GetMainViewport() { return m_MainVP; }
};

