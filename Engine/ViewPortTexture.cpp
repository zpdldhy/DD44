#include "pch.h"
#include "ViewPortTexture.h"
#include "Device.h"

void ViewPortTexture::CreateViewPortTexture(FLOAT _fWidth, FLOAT _fHeight)
{
	SetViewPort(_fWidth, _fHeight);

	m_TexDesc.Width = _fWidth;
	m_TexDesc.Height = _fHeight;
	m_TexDesc.MipLevels = 1;
	m_TexDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	m_TexDesc.SampleDesc.Count = 1;
	m_TexDesc.SampleDesc.Quality = 0;
	m_TexDesc.Usage = D3D11_USAGE_DEFAULT;
	m_TexDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	m_TexDesc.CPUAccessFlags = 0;
	m_TexDesc.MiscFlags = 0;
	m_TexDesc.ArraySize = 1;

	// Create Texture
	HRESULT hr = DEVICE->CreateTexture2D(&m_TexDesc, NULL, m_pTexture.GetAddressOf());

	if (FAILED(hr))
	{
		DX_CHECK(hr, _T(__FUNCTION__));
	}

	// Create ShaderResourceView
	// Material에서 texture로 가지게 되는 부분
	hr = DEVICE->CreateShaderResourceView(m_pTexture.Get(), NULL, m_pTexSRV.GetAddressOf());

	if (FAILED(hr))
	{
		DX_CHECK(hr, _T(__FUNCTION__));
	}

	// Create RenderTargetView
	// ViewPort를 바꾸고 세팅하는 역할
	hr = DEVICE->CreateRenderTargetView(m_pTexture.Get(), NULL, m_pRenderTargetView.GetAddressOf());

	if (FAILED(hr))
	{
		DX_CHECK(hr, _T(__FUNCTION__));
	}

	// DepthStencilView Set
	ComPtr<ID3D11Texture2D> pDSTexture = nullptr;
	D3D11_TEXTURE2D_DESC DescDepth;
	ZeroMemory(&DescDepth, sizeof(D3D11_TEXTURE2D_DESC));
	DescDepth.Width = _fWidth;
	DescDepth.Height = _fHeight;
	DescDepth.MipLevels = 1;
	DescDepth.ArraySize = 1;
	DescDepth.Format = DXGI_FORMAT_R24G8_TYPELESS;
	DescDepth.SampleDesc.Count = 1;
	DescDepth.SampleDesc.Quality = 0;
	DescDepth.CPUAccessFlags = 0;
	DescDepth.MiscFlags = 0;

	DescDepth.Usage = D3D11_USAGE_DEFAULT;
	// 백 버퍼 깊이 및 스텐실 버퍼 생성
	DescDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	if (DescDepth.Format == DXGI_FORMAT_R24G8_TYPELESS || DescDepth.Format == DXGI_FORMAT_R32_TYPELESS)
	{
		// 깊이맵 전용 깊이맵 생성
		DescDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	}

	hr = DEVICE->CreateTexture2D(&DescDepth, NULL, pDSTexture.GetAddressOf());

	if (FAILED(hr))
	{
		DX_CHECK(hr, _T(__FUNCTION__));
	}

	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&dsvDesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
	ZeroMemory(&srvDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));

	switch (DescDepth.Format)
	{
	case DXGI_FORMAT_R32_TYPELESS:
		dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
		srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
		break;
	case DXGI_FORMAT_R24G8_TYPELESS:
		dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		srvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
		break;
	}

	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;

	hr = DEVICE->CreateDepthStencilView(pDSTexture.Get(), &dsvDesc, m_pDepthStencilView.ReleaseAndGetAddressOf());

	if (FAILED(hr))
	{
		DX_CHECK(hr, _T(__FUNCTION__));
	}

	if (srvDesc.Format == DXGI_FORMAT_R32_FLOAT || srvDesc.Format == DXGI_FORMAT_R24_UNORM_X8_TYPELESS)
	{
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = 1;

		hr = DEVICE->CreateShaderResourceView(pDSTexture.Get(), &srvDesc, m_pDsvSRV.ReleaseAndGetAddressOf());

		if (FAILED(hr))
		{
			DX_CHECK(hr, _T(__FUNCTION__));
		}
	}
}

//void ViewPortTexture::BeginViewPort()
//{
//	m_iViewPorts = 1;
//	// Get Prev ViewPort
//	DC->RSGetViewports(&m_iViewPorts, &m_PrevVP);
//	DC->OMGetRenderTargets(1, &m_pPrevRTV, &m_pPrevDSV);
//
//	ID3D11ShaderResourceView* pNullSRV = nullptr;
//	DC->PSSetShaderResources(0, 1, &pNullSRV);
//
//	ID3D11RenderTargetView* pNullRTV = nullptr;
//	DC->OMSetRenderTargets(1, &pNullRTV, NULL);
//
//	DC->OMSetRenderTargets(1, m_pRenderTargetView.GetAddressOf(), m_pDepthStencilView.Get());
//
//	// Clear
//	ClearViewPort();
//
//	DC->RSSetViewports(1, &m_CurrentVP);
//}
//
//void ViewPortTexture::EndViewPort()
//{
//	DC->RSSetViewports(m_iViewPorts, &m_PrevVP);
//	DC->OMSetRenderTargets(1, &m_pPrevRTV, m_pPrevDSV);
//	m_pPrevRTV->Release(); m_pPrevRTV = nullptr;
//	m_pPrevDSV->Release(); m_pPrevDSV = nullptr;
//}

void ViewPortTexture::SetViewPort(FLOAT _fWidth, FLOAT _fHeight)
{
	m_CurrentVP.TopLeftX = 0;
	m_CurrentVP.TopLeftY = 0;
	m_CurrentVP.Width = _fWidth;
	m_CurrentVP.Height = _fHeight;
	m_CurrentVP.MinDepth = 0.f;
	m_CurrentVP.MaxDepth = 1.f;
}

void ViewPortTexture::ClearViewPort()
{
	const FLOAT color[] = { 0.1f, 0.25f, 0.4f, 1.0f };

	DC->ClearRenderTargetView(m_pRenderTargetView.Get(), color);
	DC->ClearDepthStencilView(m_pDepthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0, 0);
	DC->ClearDepthStencilView(m_pDepthStencilView.Get(), D3D11_CLEAR_STENCIL, 1.0, 0);
}


void ViewPortTexture::ClearViewPortTransparent()
{
	const FLOAT color[] = { 0, 0, 0, 0 };
	DC->ClearRenderTargetView(m_pRenderTargetView.Get(), color);
	DC->ClearDepthStencilView(m_pDepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}