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
		D3D11_BUFFER_DESC bd = {};
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof(CB_Blur);
		bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bd.CPUAccessFlags = 0;

		HRESULT hr = DEVICE->CreateBuffer(&bd, nullptr, m_pBlurCB.GetAddressOf());
		assert(SUCCEEDED(hr));
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

	vector<ID3D11RenderTargetView*> pNullRTV;
	pNullRTV.resize(m_iMRTCount);
	for (int i = 0; i < pNullRTV.size(); i++)
		pNullRTV[i] = nullptr;
	DC->OMSetRenderTargets(m_iMRTCount, pNullRTV.data(), NULL);

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

	// PRDefault 연산
	{
		DC->PSSetShaderResources(0, m_vSRTList.size(), m_vSRTList.data());
		m_p3DWorld->Render();
	}

	ID3D11ShaderResourceView* nullSRVs[8] = { nullptr };
	DC->PSSetShaderResources(0, 8, nullSRVs);
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


void PostProcessManager::SetSRVToSlot(int _index, const Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& _srv)
{
	if (m_vSRTList.size() <= _index)
		m_vSRTList.resize(_index + 1);

	m_vSRTList[_index] = _srv.Get();
}