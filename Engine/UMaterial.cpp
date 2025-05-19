#include "pch.h"
#include "UMaterial.h"
#include "Device.h"

void UMaterial::Load(wstring _textureFileName, wstring _shaderFileName)
{
    m_TexturePath = _textureFileName;
    m_ShaderPath = _shaderFileName;

	m_pShader = SHADER->Load(_shaderFileName);
    if (_textureFileName != L"")
        m_pTexture = TEXTURE->Load(_textureFileName);
	m_pInputlayout = INPUTLAYOUT->Get();
    
    CreateEffectCB();
    CreateRenderModeCB();
    
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

	if (m_pEffectCB)
	{
        DC->VSSetConstantBuffers(2, 1, m_pEffectCB.GetAddressOf());
        DC->PSSetConstantBuffers(2, 1, m_pEffectCB.GetAddressOf());
	}

    if (m_pRenderModeBuffer)
    {
        DC->VSSetConstantBuffers(7, 1, m_pRenderModeBuffer.GetAddressOf());
        DC->PSSetConstantBuffers(7, 1, m_pRenderModeBuffer.GetAddressOf());
    }

}

void UMaterial::CreateEffectCB()
{
    if (m_pEffectCB) return;

    D3D11_BUFFER_DESC desc = {};
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.ByteWidth = sizeof(CB_MaterialEffect);
    desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

    D3D11_SUBRESOURCE_DATA initData = {};
    initData.pSysMem = &m_tEffectData;

    HRESULT hr = DEVICE->CreateBuffer(&desc, &initData, m_pEffectCB.GetAddressOf());
    assert(SUCCEEDED(hr) && "Failed to create Glow ConstantBuffer");
}



void UMaterial::CreateRenderModeCB()
{
    if (m_pRenderModeBuffer) return;

    D3D11_BUFFER_DESC desc = {};
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.ByteWidth = sizeof(CB_RMB);
    desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

    D3D11_SUBRESOURCE_DATA initData = {};
    initData.pSysMem = &m_tRenderModeData;

    HRESULT hr = DEVICE->CreateBuffer(&desc, &initData, m_pRenderModeBuffer.GetAddressOf());
    assert(SUCCEEDED(hr) && "Failed to create RenderMode ConstantBuffer");
}


void UMaterial::SetGlowParams(float _glowPower, const Vec3 _glowColor)
{
    m_tEffectData.g_fGlowPower = _glowPower;
    m_tEffectData.g_vGlowColor = _glowColor;
    UpdateEffectBuffer();
}

void UMaterial::SetHitFlashTime(float _time)
{
    m_tEffectData.g_fHitFlashTime = _time;
    UpdateEffectBuffer();
}

void UMaterial::SetUVDistortionParams(float _strength, float _speed, float _frequency)
{
    m_tEffectData.g_fDistortionStrength = _strength;
    m_tEffectData.g_fWaveSpeed = _speed;
    m_tEffectData.g_fWaveFrequency = _frequency;
    UpdateEffectBuffer();
}

void UMaterial::SetAmbientParams(const Vec3& _coeff, float _power)
{
    m_tEffectData.g_vAmbientCoeff = _coeff;
    m_tEffectData.g_fAmbientPower = _power;
    UpdateEffectBuffer();
}

void UMaterial::SetDiffuseParams(const Vec3& _coeff, float _power)
{
    m_tEffectData.g_vDiffuseCoeff = _coeff;
    m_tEffectData.g_fDiffusePower = _power;
    UpdateEffectBuffer();
}

void UMaterial::SetSpecularParams(const Vec3& _coeff, float _shininess)
{
    m_tEffectData.g_vSpecularCoeff = _coeff;
    m_tEffectData.g_fShininess = _shininess;
    UpdateEffectBuffer();
}

void UMaterial::SetCameraPos(const Vec3& _camPos)
{
    m_tEffectData.g_vCameraPos = _camPos;
    UpdateEffectBuffer();
}


void UMaterial::SetRenderMode(ERenderMode _eMode)
{
    m_eRenderMode = _eMode;
    m_tRenderModeData.iRenderMode = static_cast<int>(_eMode);
    UpdateRenderModeBuffer();
}

void UMaterial::SetEmissiveParams(const Vec3& _color, float _power)
{
    m_tEffectData.g_vEmissiveColor = _color;
    m_tEffectData.g_fEmissivePower = _power;
    UpdateEffectBuffer();
}

void UMaterial::UpdateEffectBuffer()
{
    if (!m_pEffectCB)
        CreateEffectCB();
    else
    DC->UpdateSubresource(m_pEffectCB.Get(), 0, nullptr, &m_tEffectData, 0, 0);
}


void UMaterial::UpdateRenderModeBuffer()
{
    if (!m_pRenderModeBuffer)
        CreateRenderModeCB();
    else
        DC->UpdateSubresource(m_pRenderModeBuffer.Get(), 0, nullptr, &m_tRenderModeData, 0, 0);
}



