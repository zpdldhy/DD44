#pragma once
#include "Singleton.h"
#include "ViewPortTexture.h"

class BloomManager : public Singleton<BloomManager>
{
public:
    void Init();         // �ٿ���� RT ����
    void BeginBloomPass();                    // Emissive ��� ������ ����
    void EndBloomPass();                      // Emissive ������ ���� �� ���� ����
 

    ID3D11ShaderResourceView* GetDownScaleSRV() const { return m_pDownScaleRT->GetSRV(); }
    void SRVreset() { m_pDownScaleRT->m_pTexSRV = nullptr; }

private:
    std::unique_ptr<ViewPortTexture> m_pDownScaleRT;

    ComPtr<ID3D11RenderTargetView> m_arrPrevRTV[8];
    UINT m_iPrevRTVCount = 0;
    ComPtr<ID3D11DepthStencilView> m_pPrevDSV;
    D3D11_VIEWPORT m_tPrevVP = {};
};