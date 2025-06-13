#pragma once
#include "Singleton.h"
#include "ConstantData.h"

class ShadowManager : public Singleton<ShadowManager>
{
public:
    void Init();
    void Render();

private:
    void BeginShadowPass();
    void EndShadowPass();
    void UpdateCameraCB();

    ID3D11ShaderResourceView* GetSRV() const;

private:
    shared_ptr<class ViewPortTexture>   m_pShadowTexture;
    ComPtr<ID3D11RasterizerState>       m_pRSShadowBias;

    // Get Prev Viewport
    D3D11_VIEWPORT m_PrevVP;
    UINT m_iPrevViewPorts = 0;
    ID3D11RenderTargetView* m_pPrevRTV = nullptr;
    ID3D11DepthStencilView* m_pPrevDSV = nullptr;

    // Camera
    ComPtr<ID3D11Buffer> m_pCameraCB;
    CameraConstantData m_CameraData;
    Matrix m_matViewProj;

    UINT m_iWidth = 2048;
    UINT m_iHeight = 2048;
};

