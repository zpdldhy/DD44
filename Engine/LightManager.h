#pragma once
#include "Singleton.h"
#include "LightData.h"



class ALight;

class LightManager : public Singleton<LightManager>
{
public:
    void Init();
    void UpdateLightCB(); // CB_LightArray 업데이트
    void RegisterLight(shared_ptr<ALight> light);
    void Clear();

    const vector<shared_ptr<ALight>>& GetLights() const { return m_vLights; }

private:
    vector<shared_ptr<ALight>> m_vLights;
    ComPtr<ID3D11Buffer> m_pCBLightArray;
};

