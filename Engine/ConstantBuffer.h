#pragma once
#include "Device.h"
template<typename T>
class ConstantBuffer
{
public:
    T data;

private:
    ComPtr<ID3D11Buffer> m_pBuffer;
    UINT m_iRegister = 0;

public:
    void Create(UINT reg)
    {
        m_iRegister = reg;

        D3D11_BUFFER_DESC desc = {};
        desc.ByteWidth = sizeof(T);
        desc.Usage = D3D11_USAGE_DYNAMIC;
        desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        desc.MiscFlags = 0;

        D3D11_SUBRESOURCE_DATA initData = {};
        initData.pSysMem = &data;

        HRESULT hr = DEVICE->CreateBuffer(&desc, &initData, m_pBuffer.GetAddressOf());
        if (FAILED(hr))
        {
            DX_CHECK(hr, _T("ConstantBuffer::Create"));
        }
    }

    void Update()
    {
        D3D11_MAPPED_SUBRESOURCE mapped;
        DC->Map(m_pBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
        memcpy(mapped.pData, &data, sizeof(T));
        DC->Unmap(m_pBuffer.Get(), 0);
    }

    void Push()
    {
        DC->VSSetConstantBuffers(m_iRegister, 1, m_pBuffer.GetAddressOf());
        DC->PSSetConstantBuffers(m_iRegister, 1, m_pBuffer.GetAddressOf());
    }
};

