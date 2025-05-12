#pragma once
#include "Singleton.h"

class UStaticMeshComponent;
class Shader;

struct CB_Blur
{
	Vec2 g_vTexelSize;
	Vec2 g_vDirection;
};

class PostProcessManager : public Singleton<PostProcessManager>
{
	// Get Prev Viewport
	D3D11_VIEWPORT m_PrevVP;
	UINT m_iPrevViewPorts = 0;
	ID3D11RenderTargetView* m_pPrevRTV = nullptr;
	ID3D11DepthStencilView* m_pPrevDSV = nullptr;

public:
	// 후처리용 텍스처 및 shader초기화
	void Init(UINT width, UINT height);
	void PreRender(UINT _iViewPortCount, vector<ID3D11RenderTargetView*> _RTVList, ID3D11DepthStencilView* _DSVList, vector<D3D11_VIEWPORT> _VPList);
	void PostRender();

private:
	void ClearRTV(vector<ID3D11RenderTargetView*> _RTVList, ID3D11DepthStencilView* _DSVList);

	//2-pass 가우시안 블러 생성
	void Blur(const ComPtr<ID3D11ShaderResourceView>& input);
	//Blur 결과와 원본을 합성해 최종 렌더링
	void RenderCombine(const ComPtr<ID3D11ShaderResourceView>& sceneSRV);
	//블러 결과 텍스쳐를 외부에서 접근가능
	ComPtr<ID3D11ShaderResourceView> GetBlurResultSRV() const { return m_pResultSRV; }
	ComPtr<ID3D11Texture2D> GetBlurTexture() const { return m_pResultTex; }
	void CreateInputLayout();
private:
	void BlurPass(const ComPtr<ID3D11ShaderResourceView>& input,
		const ComPtr<ID3D11RenderTargetView>& output,
		Vec2 direction);

private:
	// Temp
	ComPtr<ID3D11Texture2D> m_pTempTex;
	ComPtr<ID3D11RenderTargetView> m_pTempRTV;
	ComPtr<ID3D11ShaderResourceView> m_pTempSRV;

	// Result
	ComPtr<ID3D11Texture2D> m_pResultTex;
	ComPtr<ID3D11RenderTargetView> m_pResultRTV;
	ComPtr<ID3D11ShaderResourceView> m_pResultSRV;

	// Constant Buffer 
	ComPtr<ID3D11Buffer> m_pBlurCB;


	// 셰이더 및 화면용 
	std::shared_ptr<Shader> m_pBlurShader;
	std::shared_ptr<Shader> m_pCombineShader;

	ComPtr<ID3D11Texture2D> m_pCombinedTex;
	ComPtr<ID3D11RenderTargetView> m_pCombinedRTV;
	ComPtr<ID3D11ShaderResourceView> m_pCombinedSRV;

};

