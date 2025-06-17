#include "pch.h"
#include "UStaticMeshResources.h"

void UStaticMeshResources::Create()
{
	CreateVertexBuffer();
	CreateIndexBuffer();
	CreateInstanceBuffer();
}

void UStaticMeshResources::Bind()
{
	if (m_vInstaceList.empty())
	{
		UINT Strides = sizeof(PNCT_VERTEX);
		UINT Offsets = 0;
		DC->IASetVertexBuffers(0, 1, m_pVertexBuffer.GetAddressOf(), &Strides, &Offsets);
		DC->IASetIndexBuffer(m_pIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
		DC->IASetPrimitiveTopology(m_Topology);
	}
	else
	{
		ID3D11Buffer* buffers[] = { m_pVertexBuffer.Get(), m_pInstansBuffer.Get()};
		UINT Strides[] = { sizeof(PNCT_VERTEX), sizeof(INSTANCE_VERTEX) };
		UINT Offsets[] = { 0, 0 };
		DC->IASetVertexBuffers(0, 2, buffers, Strides, Offsets);
		DC->IASetIndexBuffer(m_pIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
		DC->IASetPrimitiveTopology(m_Topology);
	}
}
