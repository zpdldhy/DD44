#include "pch.h"
#include "ShadowManager.h"
#include "Device.h"
#include "ViewPortTexture.h"
#include "ObjectManager.h"
#include "LightManager.h"
#include "ALight.h"
#include "Timer.h"

void ShadowManager::Init()
{
	m_pShadowTexture = make_shared<ViewPortTexture>();
	m_pShadowTexture->CreateViewPortTexture(2048, 2048);

	// Rasterizer (Depth Bias)
	D3D11_RASTERIZER_DESC rsDesc = {};
	rsDesc.FillMode = D3D11_FILL_SOLID;
	rsDesc.CullMode = D3D11_CULL_NONE;
	rsDesc.DepthBias = 1000;
	rsDesc.DepthBiasClamp = 0.0f;
	rsDesc.SlopeScaledDepthBias = 1.0f;
	rsDesc.DepthClipEnable = true;
	DEVICE->CreateRasterizerState(&rsDesc, m_pRSShadowBias.GetAddressOf());

	D3D11_BUFFER_DESC pDesc;
	ZeroMemory(&pDesc, sizeof(pDesc));
	pDesc.ByteWidth = sizeof(CameraConstantData);
	pDesc.Usage = D3D11_USAGE_DEFAULT;
	pDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	HRESULT hr = DEVICE->CreateBuffer(&pDesc, nullptr, m_pCameraCB.GetAddressOf());

	if (FAILED(hr))
	{
		DX_CHECK(hr, _T("CreateCameraBuffer Failed"));
		assert(false);
	}

	D3D11_SAMPLER_DESC sampDesc = {};
	sampDesc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL;
	sampDesc.BorderColor[0] = 1.0f;
	sampDesc.BorderColor[1] = 1.0f;
	sampDesc.BorderColor[2] = 1.0f;
	sampDesc.BorderColor[3] = 1.0f;

	hr = DEVICE->CreateSamplerState(&sampDesc, m_pShadowSampler.GetAddressOf());
	if (FAILED(hr))
	{
		DX_CHECK(hr, _T("CreateShadowSampler Failed"));
		assert(false);
	}

	D3D11_BUFFER_DESC sbDesc = {};
	sbDesc.ByteWidth = sizeof(Matrix) * 2; // View + Proj
	sbDesc.Usage = D3D11_USAGE_DEFAULT;
	sbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	hr = DEVICE->CreateBuffer(&sbDesc, nullptr, m_pShadowCB.GetAddressOf());
	if (FAILED(hr))
	{
		DX_CHECK(hr, _T("CreateShadowConstantBuffer Failed"));
		assert(false);
	}
}

void ShadowManager::Render()
{
	BeginShadowPass();
	UpdateCameraCB();



	OBJECT->RenderShadow();

	EndShadowPass();
}

void ShadowManager::BeginShadowPass()
{
	m_iPrevViewPorts = 1;
	// Get
	DC->OMGetRenderTargets(m_iPrevViewPorts, &m_pPrevRTV, &m_pPrevDSV);

	// Set Null
	ID3D11ShaderResourceView* pNullSRV = nullptr;
	DC->PSSetShaderResources(0, 1, &pNullSRV);

	ID3D11RenderTargetView* pNullRTV = nullptr;
	DC->OMSetRenderTargets(1, &pNullRTV, NULL);

	// Set
	DC->RSSetState(m_pRSShadowBias.Get());
	auto pRTV = m_pShadowTexture->GetRTV();
	DC->OMSetRenderTargets(1, &pRTV, m_pShadowTexture->GetDSV());

	DC->RSGetViewports(&m_iPrevViewPorts, &m_PrevVP);
	DC->RSSetViewports(1, &m_pShadowTexture->GetVP());

	m_pShadowTexture->ClearViewPortTransparent();
}

void ShadowManager::EndShadowPass()
{
	DC->RSSetState(nullptr);

	ID3D11ShaderResourceView* pNullSRV = nullptr;
	DC->PSSetShaderResources(0, 1, &pNullSRV);

	vector<ID3D11RenderTargetView*> pNullRTV;
	pNullRTV.resize(1);
	for (int i = 0; i < pNullRTV.size(); i++)
		pNullRTV[i] = nullptr;
	DC->OMSetRenderTargets(1, pNullRTV.data(), NULL);

	DC->RSSetViewports(m_iPrevViewPorts, &m_PrevVP);
	DC->OMSetRenderTargets(1, &m_pPrevRTV, m_pPrevDSV);
	m_pPrevRTV->Release(); m_pPrevRTV = nullptr;
	m_pPrevDSV->Release(); m_pPrevDSV = nullptr;
}

void ShadowManager::UpdateCameraCB()
{
	auto pCameraComponent = LIGHT->GetLight(0)->GetCameraComponent();;
	pCameraComponent->SetFar(500.f);
	pCameraComponent->SetNear(100.f);

	Matrix shadowView = XMMatrixTranspose(pCameraComponent->GetView());
	Matrix shadowProj = XMMatrixTranspose(pCameraComponent->GetProjection());

	m_tShadowCB.View = shadowView;
	m_tShadowCB.Proj = shadowProj;

	DC->UpdateSubresource(m_pShadowCB.Get(), 0, nullptr, &m_tShadowCB, 0, 0);
	DC->VSSetConstantBuffers(12, 1, m_pShadowCB.GetAddressOf());
	DC->PSSetConstantBuffers(12, 1, m_pShadowCB.GetAddressOf());
}

ID3D11ShaderResourceView* ShadowManager::GetSRV() const
{
	return m_pShadowTexture->GetDepthSRV();
}