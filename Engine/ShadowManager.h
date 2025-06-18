#pragma once
#include "Singleton.h"
#include "ConstantData.h"

struct CBShadow
{
    Matrix View;
    Matrix Proj;
};

class ShadowManager : public Singleton<ShadowManager>
{
    ComPtr<ID3D11Buffer> m_pShadowCB;
    CBShadow m_tShadowCB;
public:
    void Init();
    void Render();
    ID3D11ShaderResourceView* GetSRV() const;
    ComPtr<ID3D11SamplerState> GetShadowSampler() const { return m_pShadowSampler; }
    
private:
    void BeginShadowPass();
    void EndShadowPass();
    void UpdateCameraCB();

    

private:
    shared_ptr<class ViewPortTexture>   m_pShadowTexture;
    ComPtr<ID3D11RasterizerState>       m_pRSShadowBias;
    ComPtr<ID3D11SamplerState>          m_pShadowSampler;
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

