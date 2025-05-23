#include "pch.h"
#include "USkinnedMeshComponent.h"
#include "AnimTrack.h"
#include "UMaterial.h"
#include "AActor.h"
#include "MeshTransform.h"

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

	if (m_pMeshTrans)
	{
		if (!m_pMeshTrans->HasParent())
		{
			Vec3 pos = GetOwner()->GetPosition();
			m_pMeshTrans->SetWorldPosition(pos);
			Vec3 rot = GetOwner()->GetRotation();
			m_pMeshTrans->SetWorldRotation(rot);
		}
		m_pMeshTrans->UpdateMatrix();
		m_matWorld = m_pMeshTrans->GetWorldMat();
	}
	else
	{
		USceneComponent::Tick();
	}

	for (auto& child : m_vChild)
	{
		child->Tick();
	}


}

void USkinnedMeshComponent::PreRender()
{
	USceneComponent::Render();

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
