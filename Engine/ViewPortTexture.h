#pragma once
#include "Texture.h"

class ViewPortTexture : public Texture
{
protected:
	// Current Viewport
	D3D11_VIEWPORT m_CurrentVP;
	D3D11_TEXTURE2D_DESC	m_TexDesc;
	ComPtr<ID3D11Texture2D> m_pTexture = nullptr;
	ComPtr<ID3D11RenderTargetView> m_pRenderTargetView = nullptr;
	ComPtr<ID3D11DepthStencilView> m_pDepthStencilView = nullptr;
	ComPtr<ID3D11ShaderResourceView> m_pDsvSRV = nullptr;// ±Ì¿Ãπˆ∆€ ∏Æº“Ω∫∫‰

public:
	ID3D11ShaderResourceView* GetSRV() { return m_pTexSRV.Get(); }
	ID3D11RenderTargetView* GetRTV() { return m_pRenderTargetView.Get(); }
	ID3D11DepthStencilView* GetDSV() { return m_pDepthStencilView.Get(); }
	ID3D11ShaderResourceView* GetDepthSRV() { return m_pDsvSRV.Get(); }
	D3D11_VIEWPORT& GetVP() { return m_CurrentVP; }

public:
	void CreateViewPortTexture(FLOAT _fWidth, FLOAT _fHeight);
	void ClearViewPortTransparent();

private:
	void SetViewPort(FLOAT _fWidth, FLOAT _fHeight);
	void ClearViewPort();
};

