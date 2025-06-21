#include "pch.h"
#include "USkinnedMeshComponent.h"
#include "AnimTrack.h"
#include "UMaterial.h"
#include "AActor.h"

void USkinnedMeshComponent::Init()
{
	USceneComponent::Init();

	for (auto& child : m_vChild)
	{
		child->SetOwner(GetOwner());
		child->Init();
	}
}

void USkinnedMeshComponent::Tick()
{
	if (m_pBaseAnim) m_pBaseAnim->Tick();

	USceneComponent::Tick();

	for (auto& child : m_vChild)
	{
		child->Tick();
	}


}

void USkinnedMeshComponent::PreRender()
{
	USceneComponent::Render();
	
	if (m_pBaseAnim)
	{
		m_pBaseAnim->Render();
	}

	if (m_pMesh)
	{
		m_pMesh->Bind();
		// 성능에 영향이 있을까
		static_pointer_cast<USkeletalMeshResources>(m_pMesh)->UpdateBindPoseData();
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