#pragma once
#include "Singleton.h"
#include "ConstantData.h"

struct CBShadow
{
    Matrix View = Matrix();
    Matrix Proj = Matrix();
};

class ShadowManager : public Singleton<ShadowManager>
{
    ComPtr<ID3D11Buffer> m_pShadowCB = nullptr;
    CBShadow m_tShadowCB = CBShadow();
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
    shared_ptr<class ViewPortTexture>   m_pShadowTexture = nullptr;
    ComPtr<ID3D11RasterizerState>       m_pRSShadowBias = nullptr;
    ComPtr<ID3D11SamplerState>          m_pShadowSampler = nullptr;
    // Get Prev Viewport
    D3D11_VIEWPORT m_PrevVP = D3D11_VIEWPORT();
    UINT m_iPrevViewPorts = 0;
    ID3D11RenderTargetView* m_pPrevRTV = nullptr;
    ID3D11DepthStencilView* m_pPrevDSV = nullptr;

    // Camera
    ComPtr<ID3D11Buffer> m_pCameraCB = nullptr;
    CameraConstantData m_CameraData = CameraConstantData();
    Matrix m_matViewProj = Matrix();

    UINT m_iWidth = 2048;
    UINT m_iHeight = 2048;
};

