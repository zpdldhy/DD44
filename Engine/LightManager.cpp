#include "pch.h"
#include "LightManager.h"
#include "Device.h"
#include "ALight.h"
#include "ULightComponent.h"



void LightManager::Init()
{
    D3D11_BUFFER_DESC bd = {};
    bd.Usage = D3D11_USAGE_DYNAMIC;
    bd.ByteWidth = sizeof(CB_LightArray);
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    HRESULT hr = DEVICE->CreateBuffer(&bd, nullptr, m_pCBLightArray.GetAddressOf());
}

void LightManager::RegisterLight(shared_ptr<ALight> light)
{
    if (m_vLights.size() >= 4) return; // MAX_LIGHTS
    m_vLights.push_back(light);
}

void LightManager::Clear()
{
    m_vLights.clear();
}

void LightManager::UpdateLightCB()
{
    CB_LightArray data = {};

    for (int i = 0; i < m_vLights.size() && i < 4; ++i)
    {
        auto light = m_vLights[i];
        auto comp = light->GetLightComponent();

        LightData lightData = comp->GetLightData(light->GetPosition());
        data.lights[i] = lightData;
        data.iNumLights++;
    }

    D3D11_MAPPED_SUBRESOURCE mapped = {};
    DC->Map(m_pCBLightArray.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
    memcpy(mapped.pData, &data, sizeof(CB_LightArray));
    DC->Unmap(m_pCBLightArray.Get(), 0);

    DC->VSSetConstantBuffers(8, 1, m_pCBLightArray.GetAddressOf());
    DC->PSSetConstantBuffers(8, 1, m_pCBLightArray.GetAddressOf());
}