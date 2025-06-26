#pragma once
#include "Singleton.h"
#include "ConstantData.h"

class ALight;

class LightManager : public Singleton<LightManager>
{
public:
    void Init();
    void Tick();
    void Render();

private:
    void UpdateLightCB(); // CB_LightArray 업데이트

public:
    void RegisterLight(shared_ptr<ALight> light);
    void Clear();

public:
    const shared_ptr<ALight> GetLight(UINT _iIndex) const;

private:
    map<UINT, shared_ptr<ALight>> m_vLights;
    UINT m_iIndex = 0;
    ComPtr<ID3D11Buffer> m_pCBLightArray;
};

