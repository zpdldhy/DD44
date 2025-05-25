#pragma once
#include "Singleton.h"
class DxState : public Singleton<DxState>
{
public:
	// BlendState
	ComPtr<ID3D11BlendState> m_pAlphaBlend;
	ComPtr<ID3D11BlendState> m_pAdditiveBlend;
	ComPtr<ID3D11BlendState> m_pMultiplyBlend;
	ComPtr<ID3D11BlendState> m_pPremultipliedAlphaBlend;
	ComPtr<ID3D11BlendState> m_pOpaqueBlend;
	ComPtr<ID3D11BlendState> m_pDualSourceBlend;

	// SamplerState
	ComPtr<ID3D11SamplerState> m_pLinearSS;
	ComPtr<ID3D11SamplerState> m_pPointSS;
	ComPtr<ID3D11SamplerState> m_pClampSS;
	ComPtr<ID3D11SamplerState> m_pMirrorSS;
	ComPtr<ID3D11SamplerState> m_pBorderSS;

	// RasterizerState
	ComPtr<ID3D11RasterizerState> m_pRSSolid;
	ComPtr<ID3D11RasterizerState> m_pRSSolidNone;
	ComPtr<ID3D11RasterizerState> m_pRSWireFrame;

	// DepthStencilState
	ComPtr<ID3D11DepthStencilState> m_pDSSDepthEnable;
	ComPtr<ID3D11DepthStencilState> m_pDSSDepthEnableZero;
	ComPtr<ID3D11DepthStencilState> m_pDSSDepthDisableZero;
	ComPtr<ID3D11DepthStencilState> m_pDSS_StencilWrite;
	ComPtr<ID3D11DepthStencilState> m_pDSS_StencilMaskEqual;

	UINT m_iMRTNum = 0;
public:
	void Create();
	void CreateBlendStates();
	void CreateSamplerStates();
	void CreateRasterizerStates();
	void CreateDepthStencilStates();
};

