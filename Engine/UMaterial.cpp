#include "pch.h"
#include "UMaterial.h"
#include "Device.h"

void UMaterial::Load(wstring _textureFileName, wstring _shaderFileName)
{
	m_pShader = SHADER->Load(_shaderFileName);
    if (_textureFileName != L"")
        m_pTexture = TEXTURE->Load(_textureFileName);
	m_pInputlayout = INPUTLAYOUT->Get();
    CreateGlowCB();
    CreateDissolveCB();
    CreateUVDistortionCB();
    CreateCameraCB();
    CreateRenderModeCB();
    CreateEmissiveCB();
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

    if (m_pDissolveCB)
    {
        DC->PSSetConstantBuffers(3, 1, m_pDissolveCB.GetAddressOf());
    }

    if (m_pNoiseSRV)
    {
        DC->PSSetShaderResources(1, 1, m_pNoiseSRV.GetAddressOf()); 
    }

    if (m_pUVDistortionCB)
    {
        DC->PSSetConstantBuffers(4, 1, m_pUVDistortionCB.GetAddressOf()); // register(b4)
    }

    if (m_pCameraCB)
    {
        DC->VSSetConstantBuffers(6, 1, m_pCameraCB.GetAddressOf());
        DC->PSSetConstantBuffers(6, 1, m_pCameraCB.GetAddressOf());
    }

    if (m_pRenderModeBuffer)
    {
        DC->VSSetConstantBuffers(7, 1, m_pRenderModeBuffer.GetAddressOf());
        DC->PSSetConstantBuffers(7, 1, m_pRenderModeBuffer.GetAddressOf());
    }

    if (m_pEmissiveCB)
    {
        DC->PSSetConstantBuffers(10, 1, m_pEmissiveCB.GetAddressOf());
    }
}


void UMaterial::SetGlowParams(float _glowPower, const Vec3 _glowColor)
{
    m_tGlowData.g_fGlowPower = _glowPower;
    m_tGlowData.g_vGlowColor = _glowColor;
    UpdateGlowBuffer();
}

void UMaterial::SetHitFlashTime(float _time)
{
    m_tGlowData.g_fHitFlashTime = _time;
    UpdateGlowBuffer();
}

void UMaterial::SetDissolveParams(float _threshold)
{
    m_tDissolveData.g_fDissolveThreshold = _threshold;
    UpdateDissolveBuffer();
}

void UMaterial::CreateGlowCB()
{
    if (m_pGlowCB) return;

    D3D11_BUFFER_DESC desc = {};
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.ByteWidth = sizeof(CB_GLOW);
    desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

    D3D11_SUBRESOURCE_DATA initData = {};
    initData.pSysMem = &m_tGlowData;

    HRESULT hr = DEVICE->CreateBuffer(&desc, &initData, m_pGlowCB.GetAddressOf());
    assert(SUCCEEDED(hr) && "Failed to create Glow ConstantBuffer");
}

void UMaterial::CreateDissolveCB()
{
    if (m_pDissolveCB) return;

    D3D11_BUFFER_DESC desc = {};
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.ByteWidth = sizeof(CB_DISSOLVE);
    desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

    D3D11_SUBRESOURCE_DATA initData = {};
    initData.pSysMem = &m_tDissolveData;

    HRESULT hr = DEVICE->CreateBuffer(&desc, &initData, m_pDissolveCB.GetAddressOf());
    assert(SUCCEEDED(hr) && "Failed to create Dissolve ConstantBuffer");
}

void UMaterial::CreateUVDistortionCB()
{
    if (m_pUVDistortionCB) return;

    D3D11_BUFFER_DESC desc = {};
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.ByteWidth = sizeof(CB_UVDistortion);
    desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

    D3D11_SUBRESOURCE_DATA initData = {};
    initData.pSysMem = &m_tUVDistortion;

    HRESULT hr = DEVICE->CreateBuffer(&desc, &initData, m_pUVDistortionCB.GetAddressOf());
    assert(SUCCEEDED(hr) && "Failed to create UV Distortion ConstantBuffer");
}

void UMaterial::CreateCameraCB()
{
    if (m_pCameraCB) return;

    D3D11_BUFFER_DESC desc = {};
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.ByteWidth = sizeof(CB_CAMERA);
    desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

    D3D11_SUBRESOURCE_DATA initData = {};
    initData.pSysMem = &m_tCameraData;

    HRESULT hr = DEVICE->CreateBuffer(&desc, &initData, m_pCameraCB.GetAddressOf());
    assert(SUCCEEDED(hr) && "Failed to create Camera ConstantBuffer");
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

void UMaterial::CreateEmissiveCB()
{
    D3D11_BUFFER_DESC bd = {};
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(CB_EMISSIVE);
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

    D3D11_SUBRESOURCE_DATA sd = {};
    sd.pSysMem = &m_tEmissiveData;

    DEVICE->CreateBuffer(&bd, &sd, m_pEmissiveCB.GetAddressOf());
}

void UMaterial::UpdateDissolveBuffer()
{
    if (!m_pDissolveCB)
    {
        CreateDissolveCB();
    }
    else
    {
        DC->UpdateSubresource(m_pDissolveCB.Get(), 0, nullptr, &m_tDissolveData, 0, 0);
    }
}

void UMaterial::UpdateGlowBuffer()
{
    if (!m_pGlowCB)
    {
        CreateGlowCB();
    }
    else
    {
        DC->UpdateSubresource(m_pGlowCB.Get(), 0, nullptr, &m_tGlowData, 0, 0);
    }
}

void UMaterial::UpdateUVDistortionBuffer(float _deltaTime)
{

    m_tUVDistortion.g_fDistortionTime += _deltaTime * m_tUVDistortion.g_fWaveSpeed;

    m_tUVDistortion.g_fDistortionTime = fmodf(m_tUVDistortion.g_fDistortionTime, DD_PI * 2);


    if (!m_pUVDistortionCB)
        CreateUVDistortionCB();
    else
        DC->UpdateSubresource(m_pUVDistortionCB.Get(), 0, nullptr, &m_tUVDistortion, 0, 0);
}

void UMaterial::UpdateCameraBuffer()
{
    if (!m_pCameraCB)
    {
        CreateCameraCB();
    }
    else
    {
        DC->UpdateSubresource(m_pCameraCB.Get(), 0, nullptr, &m_tCameraData, 0, 0);
    }
}

void UMaterial::UpdateRenderModeBuffer()
{
    if (!m_pRenderModeBuffer)
        CreateRenderModeCB();
    else
        DC->UpdateSubresource(m_pRenderModeBuffer.Get(), 0, nullptr, &m_tRenderModeData, 0, 0);
}

void UMaterial::UpdateEmissiveBuffer()
{
    if (!m_pEmissiveCB)
        CreateEmissiveCB();

    DC->UpdateSubresource(m_pEmissiveCB.Get(), 0, nullptr, &m_tEmissiveData, 0, 0);
    DC->PSSetConstantBuffers(10, 1, m_pEmissiveCB.GetAddressOf()); // b10��
}

void UMaterial::SetUVDistortionParams(float _strength, float _speed, float _frequency)
{
    m_tUVDistortion.g_fDistortionStrength = _strength;
    m_tUVDistortion.g_fWaveSpeed = _speed;
    m_tUVDistortion.g_fWaveFrequency = _frequency;
}

void UMaterial::SetNoiseTexture(std::shared_ptr<Texture> _tex)
{
    m_pNoiseSRV = _tex->m_pTexSRV;
}


void UMaterial::SetCameraPos(const Vec3& _cameraPos)
{
    m_tCameraData.g_vCameraPos = _cameraPos;
    UpdateCameraBuffer();
}

void UMaterial::SetRenderMode(ERenderMode _eMode)
{
    m_eRenderMode = _eMode;
    m_tRenderModeData.iRenderMode = static_cast<int>(_eMode);
    UpdateRenderModeBuffer();
}

void UMaterial::SetEmissiveParams(const Vec3& _color, float _power)
{
    m_tEmissiveData.g_vEmissiveColor = _color;
    m_tEmissiveData.g_fEmissivePower = _power;
    UpdateEmissiveBuffer();
}

