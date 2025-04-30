#include "pch.h"
#include "USkinnedComponent.h"
#include "Device.h"
#include "UMaterial.h"

void USkinnedComponent::PreRender()
{
	UINT Strides[2] = { sizeof(PNCT_VERTEX), sizeof(IW_VERTEX) };
	UINT Offsets[2] = { 0, 0 };
	ID3D11Buffer* pVB[2] = { m_pVertexBuffer.Get(), m_pIwBuffer.Get() };

	DC->IASetVertexBuffers(0, 2, pVB, Strides, Offsets);
	//DC->IASetIndexBuffer(m_pIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	DC->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	if (m_pMaterial)
	{
		m_pMaterial->Bind();
	}
}

void USkinnedComponent::PostRender()
{
	if (m_vIndexList.size() <= 0)
	{
		DC->Draw(m_vVertexList.size(), 0);
	}
	else
	{
		DC->DrawIndexed(m_vIndexList.size(), 0, 0);
	}
}

void USkinnedComponent::CreateIwBuffer()
{
	D3D11_BUFFER_DESC pDesc;
	ZeroMemory(&pDesc, sizeof(pDesc));
	pDesc.ByteWidth = sizeof(IW_VERTEX) * m_vIwList.size();
	pDesc.Usage = D3D11_USAGE_DEFAULT;
	pDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA pInitialData;
	ZeroMemory(&pInitialData, sizeof(pInitialData));
	pInitialData.pSysMem = &m_vIwList.at(0);
	HRESULT hr = DEVICE->CreateBuffer(&pDesc, &pInitialData, m_pIwBuffer.GetAddressOf());
	if (FAILED(hr))
	{
		assert(false);
	}
}
