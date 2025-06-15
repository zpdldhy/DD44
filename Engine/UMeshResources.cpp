#include "pch.h"
#include "UMeshResources.h"

ComPtr<ID3D11Buffer> UMeshResources::m_pInstansBuffer = nullptr;

void UMeshResources::Init()
{
}

void UMeshResources::Tick()
{
}

void UMeshResources::Render()
{
}

void UMeshResources::Destroy()
{
}

bool UMeshResources::CreateVertexBuffer()
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

bool UMeshResources::CreateIndexBuffer()
{
	if (m_vIndexList.size() <= 0) { return true; }
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

bool UMeshResources::CreateInstanceBuffer()
{
	if (m_pInstansBuffer != nullptr) { return true; }

	D3D11_BUFFER_DESC pDesc;
	ZeroMemory(&pDesc, sizeof(pDesc));
	pDesc.ByteWidth = sizeof(INSTANCE_VERTEX) * MAX_INSTANCE_COUNT;
	pDesc.Usage = D3D11_USAGE_DYNAMIC;
	pDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	pDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	pDesc.StructureByteStride = sizeof(INSTANCE_VERTEX);

	// Map, UnMap으로 재사용하기 위해 D3D11_SUBRESOURCE_DATA 미설정

	HRESULT hr = DEVICE->CreateBuffer(&pDesc, nullptr, m_pInstansBuffer.GetAddressOf());
	if (FAILED(hr))
	{
		return false;
	}

	return true;
}

void UMeshResources::UpdateInstanceList(vector<INSTANCE_VERTEX>& _InstanceList)
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;

	m_vInstaceList = _InstanceList;

	if (SUCCEEDED(DC->Map(m_pInstansBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
	{
		memcpy(mappedResource.pData, m_vInstaceList.data(), sizeof(DirectX::XMMATRIX) * m_vInstaceList.size());
		DC->Unmap(m_pInstansBuffer.Get(), 0);
	}
}
