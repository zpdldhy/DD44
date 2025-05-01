#include "pch.h"
#include "USkinnedMeshComponent.h"
#include "UAnimation.h"
#include "UMaterial.h"

void USkinnedMeshComponent::Init()
{
}

void USkinnedMeshComponent::Tick()
{
	if (m_pAnim) m_pAnim->Tick();
}

void USkinnedMeshComponent::PreRender()
{
	if (m_pAnim) m_pAnim->Render();

	UINT Strides[2] = { sizeof(PNCT_VERTEX), sizeof(IW_VERTEX) };
	UINT Offsets[2] = { 0, 0 };
	ID3D11Buffer* pVB[2] = { m_pMesh->GetVertexBuffer().Get(), m_pMesh->GetIwBuffer().Get()};

	DC->IASetVertexBuffers(0, 2, pVB, Strides, Offsets);
	DC->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	if (m_pMaterial) { m_pMaterial->Bind(); }
}

void USkinnedMeshComponent::PostRender()
{
	if (m_pMesh->GetIndexCount() <= 0)
	{
		DC->Draw(m_pMesh->GetVertexCount(), 0);
	}
	else
	{
		DC->DrawIndexed(m_pMesh->GetIndexCount(), 0, 0);
	}
}

void USkinnedMeshComponent::Destroy()
{
}
