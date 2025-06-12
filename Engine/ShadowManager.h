#pragma once
#include "Singleton.h"

class ShadowManager : public Singleton<ShadowManager>
{
public:
    void Init();
    void UpdateLightViewProj(const Vec3& lightPos, const Vec3& targetPos);
    void BeginShadowPass();
    void EndShadowPass();

    ID3D11ShaderResourceView* GetSRV() const { return m_pSRV.Get(); }
    const Matrix& GetLightViewProj() const { return m_matViewProj; }

private:
    ComPtr<ID3D11Texture2D>             m_pShadowMap;
    ComPtr<ID3D11DepthStencilView>      m_pDSV;
    ComPtr<ID3D11ShaderResourceView>    m_pSRV;
    ComPtr<ID3D11RasterizerState>       m_pRSShadowBias;

    D3D11_VIEWPORT                      m_viewport;

    Matrix m_matView;
    Matrix m_matProj;
    Matrix m_matViewProj;

    UINT m_iWidth = 2048;
    UINT m_iHeight = 2048;
};

