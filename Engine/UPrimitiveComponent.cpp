#include "pch.h"
#include "UPrimitiveComponent.h"
#include "Device.h"
#include "UMaterial.h"

void UPrimitiveComponent::SetMaterial(shared_ptr<UMaterial> _material)
{
	m_pMaterial = _material;
}

shared_ptr<UMaterial> UPrimitiveComponent::GetMaterial()
{
	return m_pMaterial;
}

bool UPrimitiveComponent::CreateVertexBuffer()
{
	if (m_vVertexList.size() <= 0) { return true; }
	D3D11_BUFFER_DESC pDesc;
	ZeroMemory(&pDesc, sizeof(pDesc));
	pDesc.ByteWidth = static_cast<UINT>(sizeof(PNCT_VERTEX)) * static_cast<UINT>(m_vVertexList.size());
	pDesc.Usage = D3D11_USAGE_DEFAULT;
	pDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA pInitialData;
	ZeroMemory(&pInitialData, sizeof(pInitialData));
	pInitialData.pSysMem = &m_vVertexList.at(0);
	HRESULT hr = DEVICE->CreateBuffer(&pDesc, &pInitialData, m_pVertexBuffer.GetAddressOf());
	if (FAILED(hr))
	{
		return false;
	}
	return true;
}

bool UPrimitiveComponent::CreateIndexBuffer()
{
	D3D11_BUFFER_DESC pDesc;
	ZeroMemory(&pDesc, sizeof(pDesc));
	pDesc.ByteWidth = sizeof(DWORD) * m_vIndexList.size();
	pDesc.Usage = D3D11_USAGE_DEFAULT;
	pDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

	D3D11_SUBRESOURCE_DATA pInitialData;
	ZeroMemory(&pInitialData, sizeof(pInitialData));
	pInitialData.pSysMem = &m_vIndexList.at(0);
	HRESULT hr = DEVICE->CreateBuffer(&pDesc, &pInitialData, m_pIndexBuffer.GetAddressOf());
	if (FAILED(hr))
	{
		return false;
	}
	return true;
}

void UPrimitiveComponent::Render()
{
	PreRender();
	PostRender();
}

void UPrimitiveComponent::PreRender()
{
	// IA Setting
	UINT Strides = sizeof(PNCT_VERTEX);
	UINT Offsets = 0;
	DC->IASetVertexBuffers(0, 1, m_pVertexBuffer.GetAddressOf(), &Strides, &Offsets);
	DC->IASetIndexBuffer(m_pIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	DC->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Material Render
	if (m_pMaterial)
	{
		m_pMaterial->Bind();
	}
}

void UPrimitiveComponent::PostRender()
{
	if (m_vIndexList.size() <= 0)
		DC->Draw(m_vVertexList.size(), 0);
	else
		DC->DrawIndexed(m_vIndexList.size(), 0, 0);
}

