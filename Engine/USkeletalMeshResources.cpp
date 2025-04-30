#include "pch.h"
#include "USkeletalMeshResources.h"

bool USkeletalMeshResources::CreateIWBuffer()
{
	//if (m_vIWList.size() <= 0) { return true; }
	//D3D11_BUFFER_DESC pDesc;
	//ZeroMemory(&pDesc, sizeof(pDesc));
	//pDesc.ByteWidth = static_cast<UINT>(sizeof(IW_VERTEX)) * static_cast<UINT>(m_vIWList.size());
	//pDesc.Usage = D3D11_USAGE_DEFAULT;
	//pDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	//D3D11_SUBRESOURCE_DATA pInitialData;
	//ZeroMemory(&pInitialData, sizeof(pInitialData));
	//pInitialData.pSysMem = &m_vIWList.at(0);
	//HRESULT hr = DEVICE->CreateBuffer(&pDesc, &pInitialData, m_pIWBuffer.GetAddressOf());
	//if (FAILED(hr))
	//{
	//	return false;
	//}
	return true;
}

void USkeletalMeshResources::Bind()
{
	CreateVertexBuffer();
	CreateIndexBuffer();
	CreateIWBuffer();
}
