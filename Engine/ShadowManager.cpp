#include "pch.h"
#include "ShadowManager.h"
#include "Device.h"

void ShadowManager::Init()
{
	// 텍스처 생성
	D3D11_TEXTURE2D_DESC texDesc = {};
	texDesc.Width = m_iWidth;
	texDesc.Height = m_iHeight;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.Format = DXGI_FORMAT_R32_TYPELESS;
	texDesc.SampleDesc.Count = 1;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	DEVICE->CreateTexture2D(&texDesc, nullptr, m_pShadowMap.GetAddressOf());

	// DSV
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	DEVICE->CreateDepthStencilView(m_pShadowMap.Get(), &dsvDesc, m_pDSV.GetAddressOf());

	// SRV
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	DEVICE->CreateShaderResourceView(m_pShadowMap.Get(), &srvDesc, m_pSRV.GetAddressOf());

	// Viewport
	m_viewport.TopLeftX = 0;
	m_viewport.TopLeftY = 0;
	m_viewport.Width = static_cast<float>(m_iWidth);
	m_viewport.Height = static_cast<float>(m_iHeight);
	m_viewport.MinDepth = 0.0f;
	m_viewport.MaxDepth = 1.0f;

	// Rasterizer (Depth Bias)
	D3D11_RASTERIZER_DESC rsDesc = {};
	rsDesc.FillMode = D3D11_FILL_SOLID;
	rsDesc.CullMode = D3D11_CULL_BACK;
	rsDesc.DepthBias = 1000;
	rsDesc.DepthBiasClamp = 0.0f;
	rsDesc.SlopeScaledDepthBias = 1.0f;
	rsDesc.DepthClipEnable = true;
	DEVICE->CreateRasterizerState(&rsDesc, m_pRSShadowBias.GetAddressOf());
}

void ShadowManager::UpdateLightViewProj(const Vec3& lightPos, const Vec3& targetPos)
{
	m_matView = XMMatrixLookAtLH(lightPos, targetPos, Vec3(0, 1, 0));

	float orthoSize = 50.0f;
	m_matProj = DirectX::XMMatrixOrthographicLH(orthoSize, orthoSize, 1.0f, 200.0f);

	m_matViewProj = m_matView * m_matProj;
}

void ShadowManager::BeginShadowPass()
{
	DC->RSSetState(m_pRSShadowBias.Get());

	DC->OMSetRenderTargets(0, nullptr, m_pDSV.Get());
	DC->ClearDepthStencilView(m_pDSV.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
	DC->RSSetViewports(1, &m_viewport);
}

void ShadowManager::EndShadowPass()
{
	DC->RSSetState(nullptr);
}