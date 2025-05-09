#pragma once
#include "Singleton.h"

class Device : public Singleton<Device>
{
    // Device & SwapChain
    ComPtr<ID3D11Device> m_pd3dDevice;
    ComPtr<ID3D11DeviceContext> m_pd3dContext;
    ComPtr<IDXGISwapChain> m_pSwapChain;

    // Render Target View
    ComPtr<ID3D11RenderTargetView> m_pRTV;

    // Depth Stencil View
    ComPtr<ID3D11DepthStencilView> m_pDSV;
    ComPtr<ID3D11Texture2D> m_pDepthStencilTexture;

    // Viewport
    D3D11_VIEWPORT m_MainVP;

    // DxState
    bool m_bWireFrame = false;

    // Bloom
    ComPtr<ID3D11RenderTargetView> m_pBloomRTV;
    ComPtr<ID3D11ShaderResourceView> m_pBloomSRV;
    ComPtr<ID3D11Texture2D> m_pBloomTexture;

private:
    void CreateDeviceAndSwapChain();
    void CreateRenderTargetView();
    void CreateDepthStencilBuffer();
    void SetViewport();
    void SetDefaultState();
    void CreateBloomRenderTarget();

public:
    void OnResize(UINT _width, UINT _height);

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
};

