#pragma once
#include "Singleton.h"
#include "ViewPortTexture.h"

class BloomManager : public Singleton<BloomManager>
{
public:
    void Init();         // 다운스케일 RT 생성
    void BeginBloomPass();                    // Emissive 대상 렌더링 시작
    void EndBloomPass();                      // Emissive 렌더링 종료 및 상태 복원
 

    ID3D11ShaderResourceView* GetDownScaleSRV() const { return m_pDownScaleRT->GetSRV(); }
    void SRVreset() { m_pDownScaleRT->m_pTexSRV = nullptr; }

private:
    std::unique_ptr<ViewPortTexture> m_pDownScaleRT;

    ComPtr<ID3D11RenderTargetView> m_arrPrevRTV[8];
    UINT m_iPrevRTVCount = 0;
    ComPtr<ID3D11DepthStencilView> m_pPrevDSV;
    D3D11_VIEWPORT m_tPrevVP = {};
};