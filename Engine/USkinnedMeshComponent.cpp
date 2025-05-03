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

	if (m_pMesh) { m_pMesh->Bind(); }

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
