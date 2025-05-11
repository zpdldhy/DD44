#include "pch.h"
#include "UStaticMeshResources.h"

void UStaticMeshResources::Create()
{
	CreateVertexBuffer();
	CreateIndexBuffer();
}

void UStaticMeshResources::Bind()
{
	UINT Strides = sizeof(PNCT_VERTEX);
	UINT Offsets = 0;
	DC->IASetVertexBuffers(0, 1, m_pVertexBuffer.GetAddressOf(), &Strides, &Offsets);
	DC->IASetIndexBuffer(m_pIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	DC->IASetPrimitiveTopology(m_Topology);
}
