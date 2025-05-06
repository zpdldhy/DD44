#pragma once
#include "IExecute.h"
#include "MapEditorUI.h"

class TestSY : public IExecute
{
public:
	shared_ptr<class UStaticMeshComponent> m_pStaticMesh;
	shared_ptr<class APawn> m_pActor;

	shared_ptr<class ACameraActor> m_pCameraActor;

	shared_ptr<class UStaticMeshComponent> m_pSkyMesh;
	shared_ptr<class ASky> m_pSky;

	std::unique_ptr<MapEditorUI> m_pMapEditorUI;
	std::vector<std::shared_ptr<class ATerrainTileActor>> m_vTiles;

	bool m_bEditorWireframe = false;

	// ViewPort Texture
	shared_ptr<class APawn> m_pPlane;

	D3D11_VIEWPORT m_vp;
	D3D11_TEXTURE2D_DESC	m_TexDesc;
	ComPtr<ID3D11Texture2D> m_pTexture = nullptr;
	ComPtr<ID3D11ShaderResourceView> m_pSRV = nullptr;
	ComPtr<ID3D11RenderTargetView> m_pRenderTargetView = nullptr;
	ComPtr<ID3D11DepthStencilView> m_pDepthStencilView = nullptr;
	ComPtr<ID3D11ShaderResourceView> m_pDsvSRV = nullptr;// ±Ì¿Ãπˆ∆€ ∏Æº“Ω∫∫‰

	// Get Prev Viewport
	D3D11_VIEWPORT m_PrevVP;
	UINT m_iViewPorts;
	ID3D11RenderTargetView* m_pPrevRTV;
	ID3D11DepthStencilView* m_pPrevDSV;

public:
	void Init() override;
	void Update() override;
	void Render() override;
	void Destroy();

public:
	void CreateViewPortTexture();
	void BeginViewPort();
	void EndViewPort();
};

