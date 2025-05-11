#include "pch.h"
#include "UMaterial.h"
#include "Device.h"

void UMaterial::Load(wstring _textureFileName, wstring _shaderFileName)
{
	m_pShader = SHADER->Load(_shaderFileName);
	if (_textureFileName != L"")
		m_pTexture = TEXTURE->Load(_textureFileName);
	m_pInputlayout = INPUTLAYOUT->Get();
	CreateEffectCB();
	CreateCameraCB();
	CreateRenderModeCB();
	CreateMaterialCB();
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

	if (m_pNoiseSRV)
	{
		DC->PSSetShaderResources(1, 1, m_pNoiseSRV.GetAddressOf());
	}

	if (m_pInputlayout)
	{
		DC->IASetInputLayout(m_pInputlayout->m_pInputLayout.Get());
	}


	if (m_pEffectCB)
	{
		UpdateEffectBuffer();
		DC->VSSetConstantBuffers(2, 1, m_pEffectCB.GetAddressOf());
		DC->PSSetConstantBuffers(2, 1, m_pEffectCB.GetAddressOf());
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

	if (m_pCBMaterial)
	{
		UpdateMaterialBuffer();
		DC->PSSetConstantBuffers(11, 1, m_pCBMaterial.GetAddressOf());
	}
}


void UMaterial::CreateEffectCB()
{
	if (m_pEffectCB) return;

	D3D11_BUFFER_DESC desc = {};
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.ByteWidth = sizeof(CB_EFFECT);
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	D3D11_SUBRESOURCE_DATA initData = {};
	initData.pSysMem = &m_tEffectData;

	HRESULT hr = DEVICE->CreateBuffer(&desc, &initData, m_pEffectCB.GetAddressOf());
	assert(SUCCEEDED(hr) && "Failed to create CB_Effect buffer");
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

void UMaterial::CreateMaterialCB()
{
	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(CB_Material);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	HRESULT hr = DEVICE->CreateBuffer(&bd, nullptr, m_pCBMaterial.GetAddressOf());
	assert(SUCCEEDED(hr));
}


void UMaterial::UpdateEffectBuffer()
{
	if (!m_pEffectCB)
		CreateEffectCB();

	DC->UpdateSubresource(m_pEffectCB.Get(), 0, nullptr, &m_tEffectData, 0, 0);
}


void UMaterial::UpdateCameraBuffer()
{
	if (!m_pCameraCB)
		CreateCameraCB();

	DC->UpdateSubresource(m_pCameraCB.Get(), 0, nullptr, &m_tCameraData, 0, 0);

}

void UMaterial::UpdateRenderModeBuffer()
{
	if (!m_pRenderModeBuffer)
		CreateRenderModeCB();

	DC->UpdateSubresource(m_pRenderModeBuffer.Get(), 0, nullptr, &m_tRenderModeData, 0, 0);
}

void UMaterial::UpdateMaterialBuffer()
{
	if (!m_pCBMaterial)
		CreateMaterialCB();

	D3D11_MAPPED_SUBRESOURCE mapped;
	DC->Map(m_pCBMaterial.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
	memcpy(mapped.pData, &m_tMaterialData, sizeof(CB_Material));
	DC->Unmap(m_pCBMaterial.Get(), 0);
}

void UMaterial::SetGlowParams(float _glowPower, const Vec3 _glowColor)
{
	m_tEffectData.g_vGlow = Vec4(_glowPower, _glowColor.x, _glowColor.y, _glowColor.z);
}

void UMaterial::SetHitFlashTime(float _time)
{
	m_tEffectData.g_vFlashTime = Vec4(_time, 0.0f, 0.0f, 0.0f); // 나머지 패딩
}

void UMaterial::SetDissolveParams(float _threshold)
{
	m_tEffectData.g_vDissolve = Vec4(_threshold, 0.0f, 0.0f, 0.0f);
}


void UMaterial::SetUVDistortionParams(float _strength, float _speed, float _frequency)
{
	m_tEffectData.g_vDistortion.x = _strength;
	m_tEffectData.g_vDistortion.y = _speed;
	m_tEffectData.g_vDistortion.z = _frequency;
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
	m_tEffectData.g_vEmissive = Vec4(_power, _color.x, _color.y, _color.z);
}


void UMaterial::SetMaterialParams(const Vec4& _ambient, const Vec4& _diffuse, const Vec4& _specular, const Vec4& _emissive)
{
	m_tMaterialData.vMaterialAmbient = _ambient;
	m_tMaterialData.vMaterialDiffuse = _diffuse;
	m_tMaterialData.vMaterialSpecular = _specular;
	m_tMaterialData.vMaterialEmissive = _emissive;
}

void UMaterial::SetUseBloom(bool b_UseBloom)
{
	m_bUseBloom = b_UseBloom;
	m_tEffectData.g_bUseBloom = b_UseBloom ? 1 : 0;
}