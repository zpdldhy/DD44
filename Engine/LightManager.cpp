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

void LightManager::Tick()
{
    for (auto& pLight : m_vLights)
    {
        pLight.second->Tick();
    }

    UpdateLightCB();
}

void LightManager::Render()
{
    for (auto& pLight : m_vLights)
    {
        pLight.second->Render();
    }
}

void LightManager::RegisterLight(shared_ptr<ALight> light)
{
    if (m_vLights.size() >= 4) return; // MAX_LIGHTS

    light->Init();
    light->m_Index = m_iIndex;
    m_vLights.insert(make_pair(m_iIndex, light));

    m_iIndex++;
}

void LightManager::Clear()
{
    m_vLights.clear();
    m_iIndex = 0;
}

const shared_ptr<ALight>& LightManager::GetLight(UINT _iIndex) const
{
    auto inter= m_vLights.find(_iIndex);
    if (inter == m_vLights.end())
        return nullptr;

    return inter->second;
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