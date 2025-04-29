#include "pch.h"
#include "UMaterial.h"
#include "Device.h"

void UMaterial::Load(wstring _textureFileName, wstring _shaderFileName)
{
	m_pShader = SHADER->Load(_shaderFileName);
	m_pTexture = TEXTURE->Load(_textureFileName);
	m_pInputlayout = INPUTLAYOUT->Get();
}

void UMaterial::Bind()
{
    // Material Render

	if (m_pShader)
	{
        DC->VSSetShader(m_pShader->m_pVertexShader.Get(), nullptr, 0);
        DC->PSSetShader(m_pShader->m_pPixelShader.Get(), nullptr, 0);
	}

	if (m_pTexture)
	{
        DC->PSSetShaderResources(0, 1, m_pTexture->m_pTexSRV.GetAddressOf());
	}

	if (m_pInputlayout)
	{
        DC->IASetInputLayout(m_pInputlayout->m_pInputLayout.Get());
	}

	if (m_pGlowCB)
	{
        DC->VSSetConstantBuffers(2, 1, m_pGlowCB.GetAddressOf());
        DC->PSSetConstantBuffers(2, 1, m_pGlowCB.GetAddressOf());
	}
}


void UMaterial::SetGlowParams(float glowPower, const DirectX::XMFLOAT3& glowColor)
{
    CB_GLOW cbData = {};
    cbData.g_fGlowPower = glowPower;
    cbData.g_vGlowColor = glowColor;

    if (!m_pGlowCB)
    {
        D3D11_BUFFER_DESC desc = {};
        desc.Usage = D3D11_USAGE_DEFAULT;
        desc.ByteWidth = sizeof(CB_GLOW);
        desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

        D3D11_SUBRESOURCE_DATA initData = {};
        initData.pSysMem = &cbData;

        HRESULT hr = DEVICE->CreateBuffer(&desc, &initData, m_pGlowCB.GetAddressOf());
        if (FAILED(hr))
        {
            assert(false && "Glow ConstantBuffer Create Failed!");
        }
    }
    else
    {
        DC->UpdateSubresource(m_pGlowCB.Get(), 0, nullptr, &cbData, 0, 0);
    }
}