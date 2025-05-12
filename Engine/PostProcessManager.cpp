#include "pch.h"
#include "PostProcessManager.h"
#include "Device.h"
#include "Shader.h"
#include "Inputlayout.h"
#include "UStaticMeshComponent.h"

void PostProcessManager::Init(UINT width, UINT height)
{
	// 텍스처 공통 설정
	D3D11_TEXTURE2D_DESC texDesc = {};
	texDesc.Width = width;
	texDesc.Height = height;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	texDesc.SampleDesc.Count = 1;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

	// Temp 텍스처 생성
	DEVICE->CreateTexture2D(&texDesc, nullptr, m_pTempTex.GetAddressOf());
	DEVICE->CreateRenderTargetView(m_pTempTex.Get(), nullptr, m_pTempRTV.GetAddressOf());
	DEVICE->CreateShaderResourceView(m_pTempTex.Get(), nullptr, m_pTempSRV.GetAddressOf());

	// Result 텍스처 생성
	DEVICE->CreateTexture2D(&texDesc, nullptr, m_pResultTex.GetAddressOf());
	DEVICE->CreateRenderTargetView(m_pResultTex.Get(), nullptr, m_pResultRTV.GetAddressOf());
	DEVICE->CreateShaderResourceView(m_pResultTex.Get(), nullptr, m_pResultSRV.GetAddressOf());

	// Constant Buffer (Blur용)
	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(CB_Blur);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	DEVICE->CreateBuffer(&bd, nullptr, m_pBlurCB.GetAddressOf());

	// 셰이더 로드
	m_pBlurShader = SHADER->Load(L"../Resources/Shader/Blur.hlsl");
	m_pCombineShader = SHADER->Load(L"../Resources/Shader/CombineShader.hlsl");

	CreateInputLayout();
}

void PostProcessManager::PreRender(UINT _iViewPortCount, vector<ID3D11RenderTargetView*> _RTVList, ID3D11DepthStencilView* _DSVList, vector<D3D11_VIEWPORT> _VPList)
{
	m_iPrevViewPorts = 1;

	ClearRTV(_RTVList, _DSVList);
	
	DC->OMGetRenderTargets(1, &m_pPrevRTV, &m_pPrevDSV);
	DC->OMSetRenderTargets(_iViewPortCount, _RTVList.data(), _DSVList);

	//DC->RSGetViewports(&m_iPrevViewPorts, &m_PrevVP);
	//DC->OMGetRenderTargets(1, &m_pPrevRTV, &m_pPrevDSV);

	//ID3D11ShaderResourceView* pNullSRV = nullptr;
	//DC->PSSetShaderResources(0, 1, &pNullSRV);

	//ID3D11RenderTargetView* pNullRTV = nullptr;
	//DC->OMSetRenderTargets(1, &pNullRTV, NULL);

	//DC->OMSetRenderTargets(_iViewPortCount, _RTVList.data(), _DSVList);

	//ClearViewPort(_RTVList, _DSVList);

	//DC->RSSetViewports(_iViewPortCount, _VPList.data());
}

void PostProcessManager::PostRender()
{
	DC->OMSetRenderTargets(1, &m_pPrevRTV, m_pPrevDSV);

	//ID3D11ShaderResourceView* pNullSRV = nullptr;
	//DC->PSSetShaderResources(0, 1, &pNullSRV);

	//ID3D11RenderTargetView* pNullRTV = nullptr;
	//DC->OMSetRenderTargets(1, &pNullRTV, NULL);

	//DC->RSSetViewports(m_iPrevViewPorts, &m_PrevVP);
	//DC->OMSetRenderTargets(1, &m_pPrevRTV, m_pPrevDSV);
	m_pPrevRTV->Release(); m_pPrevRTV = nullptr;
	m_pPrevDSV->Release(); m_pPrevDSV = nullptr;
}

void PostProcessManager::ClearRTV(vector<ID3D11RenderTargetView*> _RTVList, ID3D11DepthStencilView* _DSVList)
{
	const FLOAT color[] = { 0.1f, 0.25f, 0.4f, 1.0f };

	for (auto& RTV : _RTVList)
		DC->ClearRenderTargetView(RTV, color);

	//DC->ClearDepthStencilView(_DSVList, D3D11_CLEAR_DEPTH, 1.0, 0);
	//DC->ClearDepthStencilView(_DSVList, D3D11_CLEAR_STENCIL, 1.0, 0);
}

void PostProcessManager::CreateInputLayout()
{
	// Input Layout 정의 (셰이더의 입력 구조와 일치해야 함)
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION",  0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD",  0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	UINT iNumCnt = sizeof(layout) / sizeof(layout[0]);

	//// 쉐이더 코드로부터 InputLayout 생성
	//if (!m_pBlurShader->LoadInputLayout(layout, iNumCnt))
	//{
	//    DX_CHECK(E_FAIL, _T("Failed to create input layout"));
	//}
}

void PostProcessManager::Blur(const ComPtr<ID3D11ShaderResourceView>& input)
{
	BlurPass(input, m_pTempRTV, Vec2(1, 0));   // 수평 블러
	BlurPass(m_pTempSRV, m_pResultRTV, Vec2(0, 1)); // 수직 블러
}

void PostProcessManager::BlurPass(const ComPtr<ID3D11ShaderResourceView>& input, const ComPtr<ID3D11RenderTargetView>& output, Vec2 direction)
{
	// 텍셀 사이즈 계산
	D3D11_TEXTURE2D_DESC texDesc = {};
	m_pTempTex->GetDesc(&texDesc);

	CB_Blur cb = {};
	cb.g_vTexelSize = Vec2(1.0f / texDesc.Width, 1.0f / texDesc.Height);
	cb.g_vDirection = direction;

	// ConstantBuffer에 데이터 전송
	DC->UpdateSubresource(m_pBlurCB.Get(), 0, nullptr, &cb, 0, 0);
	DC->PSSetConstantBuffers(11, 1, m_pBlurCB.GetAddressOf());

	// 렌더 타겟 설정 및 Clear
	DC->OMSetRenderTargets(1, output.GetAddressOf(), nullptr);
	float clearColor[4] = { 0, 0, 0, 0 };
	DC->ClearRenderTargetView(output.Get(), clearColor);

	// 셰이더 설정
	DC->VSSetShader(m_pBlurShader->m_pVertexShader.Get(), nullptr, 0);
	DC->PSSetShader(m_pBlurShader->m_pPixelShader.Get(), nullptr, 0);

	// 인풋 텍스처 설정
	DC->PSSetShaderResources(0, 1, input.GetAddressOf());




	// SRV 해제
	ID3D11ShaderResourceView* nullSRV[1] = { nullptr };
	DC->PSSetShaderResources(0, 1, nullSRV);
}

void PostProcessManager::RenderCombine(const ComPtr<ID3D11ShaderResourceView>& sceneSRV)
{
	// 백버퍼에 렌더링 준비
	ID3D11RenderTargetView* backBufferRTV = GET_SINGLE(Device)->GetBackBufferRTV();
	ID3D11DepthStencilView* dsv = GET_SINGLE(Device)->GetDepthStencilView();
	DC->OMSetRenderTargets(1, &backBufferRTV, dsv);

	// Combine 셰이더 바인딩
	DC->VSSetShader(m_pCombineShader->m_pVertexShader.Get(), nullptr, 0);
	DC->PSSetShader(m_pCombineShader->m_pPixelShader.Get(), nullptr, 0);

	// 텍스처 바인딩 (t0 = 원본, t1 = 블러 결과)
	DC->PSSetShaderResources(0, 1, sceneSRV.GetAddressOf());
	DC->PSSetShaderResources(1, 1, m_pResultSRV.GetAddressOf());


	// SRV 해제
	ID3D11ShaderResourceView* nullSRV[2] = { nullptr, nullptr };
	DC->PSSetShaderResources(0, 2, nullSRV);
}