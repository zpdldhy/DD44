#include "pch.h"
#include "DxState.h"
#include "Device.h"

void DxState::Create()
{
	CreateBlendStates();
	CreateSamplerStates();
	CreateRasterizerStates();
	CreateDepthStencilStates();
}

void DxState::CreateBlendStates()
{
	// Alpha Blend
	{
		D3D11_BLEND_DESC bd;
		ZeroMemory(&bd, sizeof(bd));
		bd.AlphaToCoverageEnable = FALSE;
		bd.IndependentBlendEnable = TRUE;
		bd.RenderTarget[0].BlendEnable = TRUE;
		bd.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		bd.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		bd.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		bd.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		bd.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		bd.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		bd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

		HRESULT hr = DEVICE->CreateBlendState(&bd, m_pAlphaBlend.GetAddressOf());
		if (FAILED(hr))
		{
			DX_CHECK(hr, _T(__FUNCTION__));
		}
	}

	// Additive Blend (빛나는 효과, 파티클용)
	{
		D3D11_BLEND_DESC bd;
		ZeroMemory(&bd, sizeof(bd));
		bd.AlphaToCoverageEnable = FALSE;
		bd.IndependentBlendEnable = TRUE;
		bd.RenderTarget[0].BlendEnable = TRUE;
		bd.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
		bd.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
		bd.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		bd.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		bd.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		bd.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		bd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

		HRESULT hr = DEVICE->CreateBlendState(&bd, m_pAdditiveBlend.GetAddressOf());
		if (FAILED(hr))
		{
			DX_CHECK(hr, _T(__FUNCTION__));
		}
	}

	// Multiply Blend (그림자, 어두운 합성 효과)
	{
		D3D11_BLEND_DESC bd;
		ZeroMemory(&bd, sizeof(bd));
		bd.AlphaToCoverageEnable = FALSE;
		bd.IndependentBlendEnable = TRUE;
		bd.RenderTarget[0].BlendEnable = TRUE;
		bd.RenderTarget[0].SrcBlend = D3D11_BLEND_DEST_COLOR;
		bd.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
		bd.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		bd.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		bd.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		bd.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		bd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

		HRESULT hr = DEVICE->CreateBlendState(&bd, m_pMultiplyBlend.GetAddressOf());
		if (FAILED(hr))
		{
			DX_CHECK(hr, _T(__FUNCTION__));
		}
	}

	// Premultiplied Alpha Blend (프리멀티 알파 텍스처용)
	{
		D3D11_BLEND_DESC bd;
		ZeroMemory(&bd, sizeof(bd));
		bd.AlphaToCoverageEnable = FALSE;
		bd.IndependentBlendEnable = TRUE;
		bd.RenderTarget[0].BlendEnable = TRUE;
		bd.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
		bd.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		bd.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		bd.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		bd.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		bd.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		bd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

		HRESULT hr = DEVICE->CreateBlendState(&bd, m_pPremultipliedAlphaBlend.GetAddressOf());
		if (FAILED(hr))
		{
			DX_CHECK(hr, _T(__FUNCTION__));
		}
	}

	// Opaque (블렌딩 비활성화, 완전 불투명용)
	{
		D3D11_BLEND_DESC bd;
		ZeroMemory(&bd, sizeof(bd));
		bd.AlphaToCoverageEnable = FALSE;
		bd.IndependentBlendEnable = TRUE;
		bd.RenderTarget[0].BlendEnable = FALSE;
		bd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

		HRESULT hr = DEVICE->CreateBlendState(&bd, m_pOpaqueBlend.GetAddressOf());
		if (FAILED(hr))
		{
			DX_CHECK(hr, _T(__FUNCTION__));
		}
	}
}

void DxState::CreateSamplerStates()
{
	{
		// LinearSS (선형 보간)
		D3D11_SAMPLER_DESC sd;
		ZeroMemory(&sd, sizeof(sd));
		sd.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		sd.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		sd.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		sd.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;

		HRESULT hr = DEVICE->CreateSamplerState(&sd, m_pLinearSS.GetAddressOf());

		if (FAILED(hr))
		{
			DX_CHECK(hr, _T(__FUNCTION__));
		}
	}

	{
		// PointSS (최근접 이웃 보간)
		D3D11_SAMPLER_DESC sd;
		ZeroMemory(&sd, sizeof(sd));
		sd.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
		sd.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		sd.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		sd.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;

		HRESULT hr = DEVICE->CreateSamplerState(&sd, m_pPointSS.GetAddressOf());

		if (FAILED(hr))
		{
			DX_CHECK(hr, _T(__FUNCTION__));
		}
	}

	{
		// ClampSS
		D3D11_SAMPLER_DESC sd;
		ZeroMemory(&sd, sizeof(sd));
		sd.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		sd.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
		sd.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
		sd.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;

		HRESULT hr = DEVICE->CreateSamplerState(&sd, m_pClampSS.GetAddressOf());

		if (FAILED(hr))
		{
			DX_CHECK(hr, _T(__FUNCTION__));
		}
	}

	{
		// MirrorSS
		D3D11_SAMPLER_DESC sd;
		ZeroMemory(&sd, sizeof(sd));
		sd.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		sd.AddressU = D3D11_TEXTURE_ADDRESS_MIRROR;
		sd.AddressV = D3D11_TEXTURE_ADDRESS_MIRROR;
		sd.AddressW = D3D11_TEXTURE_ADDRESS_MIRROR;

		HRESULT hr = DEVICE->CreateSamplerState(&sd, m_pMirrorSS.GetAddressOf());

		if (FAILED(hr))
		{
			DX_CHECK(hr, _T(__FUNCTION__));
		}
	}

	{
		// BorderSS
		D3D11_SAMPLER_DESC sd;
		ZeroMemory(&sd, sizeof(sd));
		sd.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		sd.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
		sd.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
		sd.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
		sd.BorderColor[0] = 0.0f; // R
		sd.BorderColor[1] = 0.0f; // G
		sd.BorderColor[2] = 0.0f; // B
		sd.BorderColor[3] = 1.0f; // A

		HRESULT hr = DEVICE->CreateSamplerState(&sd, m_pBorderSS.GetAddressOf());

		if (FAILED(hr))
		{
			DX_CHECK(hr, _T(__FUNCTION__));
		}
	}
}

void DxState::CreateRasterizerStates()
{
	{
		// - CULL_BACK -> m_pRSSolid (기본)
		D3D11_RASTERIZER_DESC rsDesc;
		ZeroMemory(&rsDesc, sizeof(rsDesc));
		rsDesc.FillMode = D3D11_FILL_SOLID;
		rsDesc.CullMode = D3D11_CULL_FRONT;
		rsDesc.FrontCounterClockwise = TRUE;
		rsDesc.DepthClipEnable = true;
		HRESULT hr = DEVICE->CreateRasterizerState(&rsDesc, m_pRSSolid.GetAddressOf());
		if (FAILED(hr))
		{
			DX_CHECK(hr, _T(__FUNCTION__));
		}
	}

	{
		// - CULL_NONE -> m_pRSSolidNone (투명 오브젝트, SkyBox)
		D3D11_RASTERIZER_DESC rsDesc;
		ZeroMemory(&rsDesc, sizeof(rsDesc));
		rsDesc.FillMode = D3D11_FILL_SOLID;
		rsDesc.CullMode = D3D11_CULL_NONE;
		rsDesc.FrontCounterClockwise = TRUE;
		rsDesc.DepthClipEnable = false;
		HRESULT hr = DEVICE->CreateRasterizerState(&rsDesc, m_pRSSolidNone.GetAddressOf());
		if (FAILED(hr))
		{
			DX_CHECK(hr, _T(__FUNCTION__));
		}
	}

	{
		// - WIRE_FRAME (디버깅/프리뷰)
		D3D11_RASTERIZER_DESC rsDesc;
		ZeroMemory(&rsDesc, sizeof(rsDesc));
		rsDesc.FillMode = D3D11_FILL_WIREFRAME;
		rsDesc.CullMode = D3D11_CULL_NONE;
		rsDesc.FrontCounterClockwise = TRUE;
		HRESULT hr = DEVICE->CreateRasterizerState(&rsDesc, m_pRSWireFrame.GetAddressOf());
		if (FAILED(hr))
		{
			DX_CHECK(hr, _T(__FUNCTION__));
		}
	}
}

void DxState::CreateDepthStencilStates()
{
	{
		// - Depth Enable, MASK_ALL (기본)
		D3D11_DEPTH_STENCIL_DESC dsDesc;
		ZeroMemory(&dsDesc, sizeof(dsDesc));
		dsDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
		dsDesc.StencilEnable = FALSE;
		dsDesc.DepthEnable = TRUE;
		dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		HRESULT hr = DEVICE->CreateDepthStencilState(&dsDesc, m_pDSSDepthEnable.GetAddressOf());
		if (FAILED(hr))
		{
			DX_CHECK(hr, _T(__FUNCTION__));
		}
	}

	{
		// - Depth Enable, MASK_ZERO (투명 오브젝트)
		D3D11_DEPTH_STENCIL_DESC dsDesc;
		ZeroMemory(&dsDesc, sizeof(dsDesc));
		dsDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
		dsDesc.StencilEnable = FALSE;
		dsDesc.DepthEnable = TRUE;
		dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
		HRESULT hr = DEVICE->CreateDepthStencilState(&dsDesc, m_pDSSDepthEnableZero.GetAddressOf());
		if (FAILED(hr))
		{
			DX_CHECK(hr, _T(__FUNCTION__));
		}
	}

	{
		// - Depth Disable, MASK_ZERO (UI, HUD, Skybox)
		D3D11_DEPTH_STENCIL_DESC dsDesc;
		ZeroMemory(&dsDesc, sizeof(dsDesc));
		dsDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
		dsDesc.StencilEnable = FALSE;
		dsDesc.DepthEnable = FALSE;
		dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
		HRESULT hr = DEVICE->CreateDepthStencilState(&dsDesc, m_pDSSDepthDisableZero.GetAddressOf());
		if (FAILED(hr))
		{
			DX_CHECK(hr, _T(__FUNCTION__));
		}
	}
}
