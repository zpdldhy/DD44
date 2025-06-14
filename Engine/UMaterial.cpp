#include "pch.h"
#include "UMaterial.h"
#include "Device.h"

void UMaterial::Load(wstring _textureFileName, wstring _shaderFileName)
{
    m_TexturePath = _textureFileName;
    m_ShaderPath = _shaderFileName;

	m_pShader = SHADER->Get(_shaderFileName);
    if (_textureFileName != L"")
        m_pTexture = TEXTURE->Get(_textureFileName);
	m_pInputlayout = INPUTLAYOUT->Get();
    
    CreateEffectCB();
    m_CB_SpriteUV = make_shared<ConstantBuffer<CB_SpriteUV>>();
    m_CB_SpriteUV->Create(4); 
    CreateSlashCB();
    
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

    if (m_CB_SpriteUV)
    {
        m_CB_SpriteUV->Push();  
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

void UMaterial::CreateSlashCB()
{
    D3D11_BUFFER_DESC desc = {};
    desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    desc.ByteWidth = sizeof(CB_Slash);
    desc.Usage = D3D11_USAGE_DYNAMIC;
    desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    HRESULT hr = DEVICE->CreateBuffer(&desc, nullptr, m_pCB_Slash.GetAddressOf());
    if (FAILED(hr))
    {
        int a = 0;
    }
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

void UMaterial::SetEmissiveParams(const Vec3& _color, float _power)
{
    m_tEffectData.g_vEmissiveColor = _color;
    m_tEffectData.g_fEmissivePower = _power;
    UpdateEffectBuffer();
}


void UMaterial::SetIsMetal(bool _bMetal, float _shininess)
{
    if (_bMetal)
    {
        m_tEffectData.g_vSpecularCoeff = { 1,1,1 };
        m_tEffectData.g_fShininess = _shininess;
    }
    
    UpdateEffectBuffer();
}


void UMaterial::SetTintColor(const Vec4& color)
{
    m_tEffectData.TintColor = color;
    UpdateEffectBuffer();
}

void UMaterial::SetCrash(bool _bCrash)
{
    m_tEffectData.g_bCrash = _bCrash;
    UpdateEffectBuffer();
}

void UMaterial::SetDissolve(float _amount)
{
    m_tEffectData.g_fDissolveAmount = _amount;
    m_tEffectData.g_fDissolveEdgeWidth = 0.05f;
    m_tEffectData.g_vDissolveColor = Vec3(1, 0.3f, 0.0f);
    UpdateEffectBuffer();
}

void UMaterial::UpdateEffectBuffer()
{
    if (!m_pEffectCB)
        CreateEffectCB();
    else
    DC->UpdateSubresource(m_pEffectCB.Get(), 0, nullptr, &m_tEffectData, 0, 0);
}

void UMaterial::SetUVRange(Vec2 start, Vec2 end)
{
    if (!m_CB_SpriteUV) return;

    m_CB_SpriteUV->data.uvStart = start;
    m_CB_SpriteUV->data.uvEnd = end;
    m_CB_SpriteUV->Update();
    m_CB_SpriteUV->Push();
}

void UMaterial::SetSlashProgress(float _progress)
{
    m_tSlashData.g_fProgress = _progress;

    D3D11_MAPPED_SUBRESOURCE mapped = {};
    DC->Map(m_pCB_Slash.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
    memcpy(mapped.pData, &m_tSlashData, sizeof(CB_Slash));
    DC->Unmap(m_pCB_Slash.Get(), 0);

    // 바인딩도 여기서 바로 해도 됨
    DC->VSSetConstantBuffers(10, 1, m_pCB_Slash.GetAddressOf());
    DC->PSSetConstantBuffers(10, 1, m_pCB_Slash.GetAddressOf());
}
