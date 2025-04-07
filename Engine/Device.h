#pragma once
#include "Singleton.h"

class Device : public Singleton<Device>
{
    ComPtr<ID3D11Device> _pd3dDevice;
    ComPtr<ID3D11DeviceContext> _pd3dContext;
    ComPtr<IDXGISwapChain> _pSwapChain;
    ComPtr<ID3D11RenderTargetView> _pRTV;
    ComPtr<ID3D11DepthStencilView> _pDSV;
    ComPtr<ID3D11Texture2D> _pDepthStencilTexture;

    D3D11_VIEWPORT			_MainVP;
private:
    bool   CreateDevice();
    bool   CreateDepthStencilBuffer();
public:
    void   Init();
    void   Frame();
    void   PreRender();
    void   Render();
    void   PostRender();
    void   Release();
public:
    ComPtr<ID3D11Device> GetDevice();
    ComPtr< IDXGISwapChain> GetSwapChain();
    ComPtr<ID3D11DeviceContext> GetDeviceContext();
};

