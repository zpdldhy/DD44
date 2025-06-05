#pragma once
#include "Singleton.h"
#include "RenderStateManager.h"
class AWindActor;
class AWindQuadActor;
class ViewPortTexture;

class WindManager : public Singleton<WindManager>
{
private:
    vector<shared_ptr<AWindActor>> m_vWindList;

    unique_ptr<ViewPortTexture> m_pWindTarget;
    unique_ptr<ViewPortTexture> m_pWindCompositeTarget;
    ComPtr<ID3D11RenderTargetView> m_pPrevRTV;

    shared_ptr<AWindQuadActor> m_pQuadActor;
public:
    void Init();
    void Tick();
    void Render();
    void Destroy();
    void Present();

    void PreRender(); 
    void PostRender();

    void AddWind(shared_ptr<AWindActor> wind);
    void CompositeQuadToTexture();
    ID3D11ShaderResourceView* GetCompositeSRV();

    ID3D11ShaderResourceView* GetSRV(); 

    shared_ptr<AWindQuadActor> GetQuadActor() const { return m_pQuadActor; }

private:
    RenderOption m_sharedOption;
};
