#include "pch.h"
#include "PostProcessManager.h"
#include "Device.h"
#include "Shader.h"
#include "Inputlayout.h"
#include "UStaticMeshComponent.h"

void PostProcessManager::Init(UINT width, UINT height)
{
	// �ؽ�ó ���� ����
	D3D11_TEXTURE2D_DESC texDesc = {};
	texDesc.Width = width;
	texDesc.Height = height;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	texDesc.SampleDesc.Count = 1;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

	// Temp �ؽ�ó ����
	DEVICE->CreateTexture2D(&texDesc, nullptr, m_pTempTex.GetAddressOf());
	DEVICE->CreateRenderTargetView(m_pTempTex.Get(), nullptr, m_pTempRTV.GetAddressOf());
	DEVICE->CreateShaderResourceView(m_pTempTex.Get(), nullptr, m_pTempSRV.GetAddressOf());

	// Result �ؽ�ó ����
	DEVICE->CreateTexture2D(&texDesc, nullptr, m_pResultTex.GetAddressOf());
	DEVICE->CreateRenderTargetView(m_pResultTex.Get(), nullptr, m_pResultRTV.GetAddressOf());
	DEVICE->CreateShaderResourceView(m_pResultTex.Get(), nullptr, m_pResultSRV.GetAddressOf());

	// Constant Buffer (Blur��)
	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(CB_Blur);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	DEVICE->CreateBuffer(&bd, nullptr, m_pBlurCB.GetAddressOf());

	// ���̴� �ε�
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
	// Input Layout ���� (���̴��� �Է� ������ ��ġ�ؾ� ��)
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION",  0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD",  0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	UINT iNumCnt = sizeof(layout) / sizeof(layout[0]);

	//// ���̴� �ڵ�κ��� InputLayout ����
	//if (!m_pBlurShader->LoadInputLayout(layout, iNumCnt))
	//{
	//    DX_CHECK(E_FAIL, _T("Failed to create input layout"));
	//}
}

void PostProcessManager::Blur(const ComPtr<ID3D11ShaderResourceView>& input)
{
	BlurPass(input, m_pTempRTV, Vec2(1, 0));   // ���� ��
	BlurPass(m_pTempSRV, m_pResultRTV, Vec2(0, 1)); // ���� ��
}

void PostProcessManager::BlurPass(const ComPtr<ID3D11ShaderResourceView>& input, const ComPtr<ID3D11RenderTargetView>& output, Vec2 direction)
{
	// �ؼ� ������ ���
	D3D11_TEXTURE2D_DESC texDesc = {};
	m_pTempTex->GetDesc(&texDesc);

	CB_Blur cb = {};
	cb.g_vTexelSize = Vec2(1.0f / texDesc.Width, 1.0f / texDesc.Height);
	cb.g_vDirection = direction;

	// ConstantBuffer�� ������ ����
	DC->UpdateSubresource(m_pBlurCB.Get(), 0, nullptr, &cb, 0, 0);
	DC->PSSetConstantBuffers(11, 1, m_pBlurCB.GetAddressOf());

	// ���� Ÿ�� ���� �� Clear
	DC->OMSetRenderTargets(1, output.GetAddressOf(), nullptr);
	float clearColor[4] = { 0, 0, 0, 0 };
	DC->ClearRenderTargetView(output.Get(), clearColor);

	// ���̴� ����
	DC->VSSetShader(m_pBlurShader->m_pVertexShader.Get(), nullptr, 0);
	DC->PSSetShader(m_pBlurShader->m_pPixelShader.Get(), nullptr, 0);

	// ��ǲ �ؽ�ó ����
	DC->PSSetShaderResources(0, 1, input.GetAddressOf());




	// SRV ����
	ID3D11ShaderResourceView* nullSRV[1] = { nullptr };
	DC->PSSetShaderResources(0, 1, nullSRV);
}

void PostProcessManager::RenderCombine(const ComPtr<ID3D11ShaderResourceView>& sceneSRV)
{
	// ����ۿ� ������ �غ�
	ID3D11RenderTargetView* backBufferRTV = GET_SINGLE(Device)->GetBackBufferRTV();
	ID3D11DepthStencilView* dsv = GET_SINGLE(Device)->GetDepthStencilView();
	DC->OMSetRenderTargets(1, &backBufferRTV, dsv);

	// Combine ���̴� ���ε�
	DC->VSSetShader(m_pCombineShader->m_pVertexShader.Get(), nullptr, 0);
	DC->PSSetShader(m_pCombineShader->m_pPixelShader.Get(), nullptr, 0);

	// �ؽ�ó ���ε� (t0 = ����, t1 = �� ���)
	DC->PSSetShaderResources(0, 1, sceneSRV.GetAddressOf());
	DC->PSSetShaderResources(1, 1, m_pResultSRV.GetAddressOf());


	// SRV ����
	ID3D11ShaderResourceView* nullSRV[2] = { nullptr, nullptr };
	DC->PSSetShaderResources(0, 2, nullSRV);
}