#include "pch.h"
#include "PostProcessManager.h"
#include "Device.h"
#include "Shader.h"
#include "Inputlayout.h"
#include "UStaticMeshComponent.h"
#include "ViewPortTexture.h"
#include "AActor.h"
#include "UMaterial.h"

void PostProcessManager::Init(UINT _count)
{
	m_iMRTCount = _count;

	for (int i = 0; i < _count; i++)
	{
		auto pMRT = make_shared<ViewPortTexture>();
		pMRT->CreateViewPortTexture(static_cast<FLOAT>(g_windowSize.x), static_cast<FLOAT>(g_windowSize.y));
		m_vMRTList.emplace_back(pMRT);
		m_vRTVList.emplace_back(pMRT->GetRTV());
		m_vVPList.emplace_back(pMRT->GetVP());
		m_vSRTList.emplace_back(pMRT->GetSRV());
	}

	CreatePostProcessor();
}

void PostProcessManager::PreRender()
{
	m_p3DWorld->Tick();

	m_iPrevViewPorts = 1;

	DC->OMGetRenderTargets(m_iPrevViewPorts, &m_pPrevRTV, &m_pPrevDSV);

	ID3D11ShaderResourceView* pNullSRV = nullptr;
	DC->PSSetShaderResources(0, 1, &pNullSRV);

	ID3D11RenderTargetView* pNullRTV = nullptr;
	DC->OMSetRenderTargets(1, &pNullRTV, NULL);

	DC->OMSetRenderTargets(m_vMRTList.size(), m_vRTVList.data(), m_vMRTList[0]->GetDSV());

	DC->RSGetViewports(&m_iPrevViewPorts, &m_PrevVP);
	DC->RSSetViewports(m_vMRTList.size(), m_vVPList.data());

	ClearRTV(m_vRTVList, m_vMRTList[0]->GetDSV());
}

void PostProcessManager::PostRender()
{
	ID3D11ShaderResourceView* pNullSRV = nullptr;
	DC->PSSetShaderResources(0, 1, &pNullSRV);

	ID3D11RenderTargetView* pNullRTV = nullptr;
	DC->OMSetRenderTargets(1, &pNullRTV, NULL);

	DC->RSSetViewports(m_iPrevViewPorts, &m_PrevVP);
	DC->OMSetRenderTargets(1, &m_pPrevRTV, m_pPrevDSV);
	m_pPrevRTV->Release(); m_pPrevRTV = nullptr;
	m_pPrevDSV->Release(); m_pPrevDSV = nullptr;
}

void PostProcessManager::Present()
{
	DC->PSSetShaderResources(0, m_vSRTList.size(), m_vSRTList.data());

	m_p3DWorld->Render();

	ID3D11ShaderResourceView* nullSRV[3] = { nullptr, };
	DC->PSSetShaderResources(0, m_vSRTList.size(), nullSRV);
}

void PostProcessManager::CreatePostProcessor()
{
	float fWinSizeX = static_cast<float>(g_windowSize.x);
	float fWinSizeY = static_cast<float>(g_windowSize.y);

	m_p3DWorld = make_shared<AActor>();

	auto pMesh = UStaticMeshComponent::CreatePlane();

	auto pMaterial = make_shared<UMaterial>();
	pMaterial->Load(L"", L"../Resources/Shader/PRDefault.hlsl");
	pMesh->SetMaterial(pMaterial);

	m_p3DWorld->SetMeshComponent(pMesh);
	m_p3DWorld->SetPosition(Vec3(0.f, 0.f, 1.f));
	m_p3DWorld->SetScale(Vec3(fWinSizeX, fWinSizeY, 0.f));
	m_p3DWorld->Init();
}

void PostProcessManager::ClearRTV(vector<ID3D11RenderTargetView*> _RTVList, ID3D11DepthStencilView* _DSVList)
{
	const FLOAT color[] = { 0.1f, 0.25f, 0.4f, 1.0f };

	for (auto& RTV : _RTVList)
		DC->ClearRenderTargetView(RTV, color);

	DC->ClearDepthStencilView(_DSVList, D3D11_CLEAR_DEPTH, 1.0, 0);
	DC->ClearDepthStencilView(_DSVList, D3D11_CLEAR_STENCIL, 1.0, 0);
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