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

	// Get Prev Viewport
	D3D11_VIEWPORT m_PrevVP;
	UINT m_iViewPorts;
	ID3D11RenderTargetView* m_pPrevRTV = nullptr;
	ID3D11DepthStencilView* m_pPrevDSV = nullptr;

public:
	ID3D11ShaderResourceView* GetSRV() const { return m_pTexSRV.Get(); }

public:
	void CreateViewPortTexture(FLOAT _fWidth, FLOAT _fHeight);
	void BeginViewPort();
	void EndViewPort();

private:
	void SetViewPort(FLOAT _fWidth, FLOAT _fHeight);
	void ClearViewPort();
};

