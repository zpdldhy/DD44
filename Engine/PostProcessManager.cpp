#include "pch.h"
#include "PostProcessManager.h"
#include "Device.h"
#include "Shader.h"
#include "Inputlayout.h"
#include "UStaticMeshComponent.h"
#include "ViewPortTexture.h"
#include "AActor.h"
#include "UMaterial.h"
#include "Dxstate.h"

void PostProcessManager::Init(UINT _count)
{
	m_iMRTCount = _count;

	for (int i = 0; i < _count; i++)
	{
		auto pMRT = make_shared<ViewPortTexture>();
		pMRT->CreateViewPortTexture(static_cast<FLOAT>(g_windowSize.x), static_cast<FLOAT>(g_windowSize.y));
		m_vMRTList.emplace_back(pMRT);
		m_vRTVList.emplace_back(pMRT->GetRTV());	// Pre, Post에서 갈아끼우는 용도
		m_vVPList.emplace_back(pMRT->GetVP());		// Pre, Post에서 갈아끼우는 용도
		m_vSRTList.emplace_back(pMRT->GetSRV());	// MRT 결과로 나오는 Texture
	}

	{
		m_pBlurShader = SHADER->Load(L"../Resources/Shader/Blur.hlsl");

		D3D11_BUFFER_DESC bd = {};
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof(CB_Blur);
		bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bd.CPUAccessFlags = 0;
		
		HRESULT hr = DEVICE->CreateBuffer(&bd, nullptr, m_pBlurCB.GetAddressOf());
		assert(SUCCEEDED(hr));

		D3D11_TEXTURE2D_DESC desc = {};
		ZeroMemory(&desc, sizeof(desc));
		desc.Width = g_windowSize.x;
		desc.Height = g_windowSize.y;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.SampleDesc.Count = 1;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

		DEVICE->CreateTexture2D(&desc, nullptr, m_pTempTex.GetAddressOf());
		DEVICE->CreateRenderTargetView(m_pTempTex.Get(), nullptr, m_pTempRTV.GetAddressOf());
		DEVICE->CreateShaderResourceView(m_pTempTex.Get(), nullptr, m_pTempSRV.GetAddressOf());

		DEVICE->CreateTexture2D(&desc, nullptr, m_pFinalTex.GetAddressOf());
		DEVICE->CreateRenderTargetView(m_pFinalTex.Get(), nullptr, m_pFinalRTV.GetAddressOf());
		DEVICE->CreateShaderResourceView(m_pFinalTex.Get(), nullptr, m_pFinalSRV.GetAddressOf());
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
	m_tBlurCB.g_vTexelSize = Vec2(1.0f / g_windowSize.x, 1.0f / g_windowSize.y);

	DC->UpdateSubresource(m_pBlurCB.Get(), 0, nullptr, &m_tBlurCB, 0, 0);
	DC->PSSetConstantBuffers(11, 1, m_pBlurCB.GetAddressOf());

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

//void PostProcessManager::BlurPass(const ComPtr<ID3D11ShaderResourceView>& _input, const ComPtr<ID3D11RenderTargetView>& _output, Vec2 _direction)
//{
//	// 1. 입력 텍스처로부터 해상도 정보 가져오기
//	ComPtr<ID3D11Resource> res;
//	_input->GetResource(res.GetAddressOf());
//
//	ComPtr<ID3D11Texture2D> tex;
//	res.As(&tex);
//
//	D3D11_TEXTURE2D_DESC desc = {};
//	tex->GetDesc(&desc);
//
//	// 2. Viewport 수동 설정
//	D3D11_VIEWPORT vp = {};
//	vp.TopLeftX = 0;
//	vp.TopLeftY = 0;
//	vp.Width = static_cast<FLOAT>(desc.Width);
//	vp.Height = static_cast<FLOAT>(desc.Height);
//	vp.MinDepth = 0.0f;
//	vp.MaxDepth = 1.0f;
//	DC->RSSetViewports(1, &vp);
//
//	// 3. Blur ConstantBuffer 업데이트
//	CB_Blur cb = {};
//	cb.g_vTexelSize = Vec2(1.0f / desc.Width, 1.0f / desc.Height);
//	cb.g_vDirection = _direction;
//	DC->UpdateSubresource(m_pBlurCB.Get(), 0, nullptr, &cb, 0, 0);
//	DC->PSSetConstantBuffers(11, 1, m_pBlurCB.GetAddressOf());
//
//	// 4. 출력용 RTV 바인딩 및 Clear
//	DC->OMSetRenderTargets(1, _output.GetAddressOf(), nullptr);
//	float clearColor[4] = { 0, 0, 0, 0 };
//	DC->ClearRenderTargetView(_output.Get(), clearColor);
//
//	// 5. Depth Test 비활성화
//	DC->OMSetDepthStencilState(STATE->m_pDSSDepthDisableZero.Get(), 0);
//
//	// 6. Shader 및 입력 리소스 바인딩
//	DC->VSSetShader(m_pBlurShader->m_pVertexShader.Get(), nullptr, 0);
//	DC->PSSetShader(m_pBlurShader->m_pPixelShader.Get(), nullptr, 0);
//	DC->PSSetShaderResources(0, 1, _input.GetAddressOf());
//
//
//
//	// 8. Depth 상태 복원
//	DC->OMSetDepthStencilState(nullptr, 0);
//
//	// 9. 리소스 정리
//	ID3D11ShaderResourceView* nullSRV[1] = { nullptr };
//	DC->PSSetShaderResources(0, 1, nullSRV);
//
//	ID3D11RenderTargetView* nullRTV = nullptr;
//	DC->OMSetRenderTargets(1, &nullRTV, nullptr);
//
//
//	ID3D11Resource* debugRes = nullptr;
//	POSTPROCESS->GetFinalSRV()->GetResource(&debugRes);
//	ID3D11Texture2D* debugTex = nullptr;
//	debugRes->QueryInterface(&debugTex);
//
//	D3D11_TEXTURE2D_DESC d = {};
//	debugTex->GetDesc(&d);
//	wchar_t buf[128];
//	swprintf_s(buf, L"[FINAL] size: %d x %d\n", d.Width, d.Height);
//	OutputDebugStringW(buf);
//
//	debugTex->Release();
//}

void PostProcessManager::SetSRVToSlot(int _index, const Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& _srv)
{
	if (m_vSRTList.size() <= _index)
		m_vSRTList.resize(_index + 1);

	m_vSRTList[_index] = _srv.Get();
}