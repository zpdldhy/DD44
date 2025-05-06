#include "pch.h"
#include "USkinnedMeshComponent.h"
#include "AnimTrack.h"
#include "UMaterial.h"

void USkinnedMeshComponent::Init()
{
}

void USkinnedMeshComponent::Tick()
{
	if (m_pBaseAnim) m_pBaseAnim->Tick();

	for (auto& child : m_vChild)
	{
		child->Tick();
	}
}

void USkinnedMeshComponent::PreRender()
{
	
	if (m_pMeshAnim) m_pMeshAnim->Render();

	if (m_pMesh) 
	{ 
		m_pMesh->Bind();
		m_pMesh->UpdateBindPoseData();
	}

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
