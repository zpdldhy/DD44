#pragma once
#include "Singleton.h"

class UStaticMeshComponent;
class Shader;
class ViewPortTexture;

struct CB_Blur
{
	Vec2 g_vTexelSize;
	Vec2 g_vDirection;
	float g_fBlurScale;
	Vec3 padding;
};

class PostProcessManager : public Singleton<PostProcessManager>
{
	UINT m_iMRTCount = 0;
	vector<shared_ptr<ViewPortTexture>> m_vMRTList;
	vector<ID3D11RenderTargetView*> m_vRTVList;
	vector<ID3D11ShaderResourceView*> m_vSRTList;
	vector<D3D11_VIEWPORT> m_vVPList;

	// MRT를 합쳐 출력하는 Actor
	shared_ptr<class AActor> m_p3DWorld = nullptr;

	// Get Prev Viewport
	D3D11_VIEWPORT m_PrevVP;
	UINT m_iPrevViewPorts = 0;
	ID3D11RenderTargetView* m_pPrevRTV = nullptr;
	ID3D11DepthStencilView* m_pPrevDSV = nullptr;

	CB_Blur m_tBlurCB = {};

public:
	// 후처리용 텍스처 및 shader초기화
	void Init(UINT _count);
	void PreRender();
	void PostRender();
	void Present();

private:
	void CreatePostProcessor();
	void ClearRTV(vector<ID3D11RenderTargetView*> _RTVList, ID3D11DepthStencilView* _DSVList);

public:
	shared_ptr<ViewPortTexture> GetMRT(UINT iIndex) { return m_vMRTList[iIndex]; }
	
	//void BlurPass(const ComPtr<ID3D11ShaderResourceView>& input,	const ComPtr<ID3D11RenderTargetView>& output,	Vec2 direction);

	ComPtr<ID3D11RenderTargetView> GetTempRTV() const { return m_pTempRTV; }
	ComPtr<ID3D11ShaderResourceView> GetTempSRV() const { return m_pTempSRV; }
	ComPtr<ID3D11RenderTargetView> GetFinalRTV() const { return m_pFinalRTV; }
	ComPtr<ID3D11ShaderResourceView> GetFinalSRV() const { return m_pFinalSRV; }
	void SetBlurScale(float _scale) { m_tBlurCB.g_fBlurScale = _scale;}

	void SetSRVToSlot(int _index, const ComPtr<ID3D11ShaderResourceView>& _srv);

private:
	// Temp -> Final 구조 (Blur용 전용 텍스처)
	ComPtr<ID3D11Texture2D> m_pTempTex;
	ComPtr<ID3D11RenderTargetView> m_pTempRTV;
	ComPtr<ID3D11ShaderResourceView> m_pTempSRV;

	// Final Blur 결과 저장
	ComPtr<ID3D11Texture2D> m_pFinalTex;
	ComPtr<ID3D11RenderTargetView> m_pFinalRTV;
	ComPtr<ID3D11ShaderResourceView> m_pFinalSRV;

	// Blur Shader 및 CBuffer
	std::shared_ptr<Shader> m_pBlurShader;
	ComPtr<ID3D11Buffer> m_pBlurCB;
};

