#include "pch.h"
#include "UInstanceSkinnedMeshComponent.h"

#include "AActor.h"
#include "UAnimInstance.h"
#include "AnimTrack.h"

void UInstanceSkinnedMeshComponent::Init()
{
	USceneComponent::Init();

	if (m_instanceId == 0)
	{
		m_pMesh->Init();
	}

	for (auto& child : m_vChild)
	{
		child->SetOwner(GetOwner());
		child->Init();
	}
}

void UInstanceSkinnedMeshComponent::Tick()
{
	if (m_pBaseAnim) m_pBaseAnim->Tick();

	USceneComponent::Tick();

	if (bRender)
	{
		UpdateMeshResourceData();
	}

	for (auto& child : m_vChild)
	{
		child->Tick();
	}

}

void UInstanceSkinnedMeshComponent::Render()
{
	if (bRender)
	{
		PreRender();
		PostRender();
	}

	for (auto& child : m_vChild)
	{
		child->Render();
	}
}

void UInstanceSkinnedMeshComponent::PreRender()
{
	if (m_pBaseAnim)
	{
		m_pBaseAnim->Render();
	}

	if (m_instanceId == 0)
	{
		if (m_pMesh)
		{
			dynamic_pointer_cast<USkeletalMeshResources>(m_pMesh)->UpdateInstanceData();
			static_pointer_cast<USkeletalMeshResources>(m_pMesh)->UpdateBindPoseData();
			m_pMesh->Bind();
		}

		if (m_pMaterial) { m_pMaterial->Bind(); }
	}
}

void UInstanceSkinnedMeshComponent::PostRender()
{
	if (m_instanceId == 0)
	{
		if (m_pMesh->GetIndexCount() <= 0)
		{
			DC->DrawInstanced(m_pMesh->GetVertexCount(), m_pMesh->GetInstanceCount(), 0, 0);
		}
		else
		{
			DC->DrawIndexed(m_pMesh->GetIndexCount(), 0, 0);
		}
	}
}

void UInstanceSkinnedMeshComponent::Destroy()
{
	for (auto& child : m_vChild)
	{
		child->Destroy();
	}

	dynamic_pointer_cast<USkeletalMeshResources>(m_pMesh)->FreeInstanceIndex(m_instanceId);
}

void UInstanceSkinnedMeshComponent::UpdateMeshResourceData()
{
	auto skinnedMesh = static_pointer_cast<USkeletalMeshResources>(m_pMesh);

	// Index 유효성 확인
	if (m_instanceId >= skinnedMesh->GetInstanceCount() || m_instanceId < 0)
	{
		m_instanceId = skinnedMesh->AllocateInstanceIndex();
	}

	// INSTANCE_VERTEX
	m_trans.matWorld = GetWorld();
	float flashTime = GetMaterial()->GetFlashTIme();
	float isCrack = (GetMaterial()->GetCrack() ? 1.0f : 0.0f);
	float dissolve = GetMaterial()->GetDissolve();
	m_trans.color = { flashTime, isCrack, dissolve, 0 };
	m_trans.uv = { 0, 0 };

	// ANIM_VERTEX
	if (m_pBaseAnim)
	{
		m_anim.track = static_cast<UINT>(m_pBaseAnim->GetCurrentIndex());
		m_anim.frame = static_cast<UINT>(m_pBaseAnim->GetCurrentFrame());
		auto pos = m_pBaseAnim->GetRootPos();
		m_anim.rootPos.x = pos.x;
		m_anim.rootPos.y = pos.y;
		m_anim.rootPos.z = pos.z;
	}
	else
	{
		auto base = m_pMeshAnim->animInstance;
		m_anim.track = static_cast<UINT>(base->GetCurrentIndex());
		m_anim.frame = static_cast<UINT>(base->GetCurrentFrame());
		auto pos = base->GetRootPos();
		m_anim.rootPos.x = pos.x;
		m_anim.rootPos.y = pos.y;
		m_anim.rootPos.z = pos.z;
	}

	static_pointer_cast<USkeletalMeshResources>(m_pMesh)->AddInstanceData(m_instanceId, m_trans, m_anim);
}

void UInstanceSkinnedMeshComponent::RenderShadow()
{
	if (bRenderShadow)
	{
		PreRender();
		//		auto shadowShader = SHADER->Get(L"../Resources/Shader/Shadow.hlsl");
		auto shadowShader = SHADER->Get(GetMaterial()->GetShaderPath(), L"VS_SHADOW", L"PS_SHADOW");
		DC->VSSetShader(shadowShader->GetVS().Get(), nullptr, 0);
		DC->PSSetShader(shadowShader->GetPS().Get(), nullptr, 0);
		PostRender();

	}

	for (auto& child : m_vChild)
	{
		child->RenderShadow();
	}
}

void UInstanceSkinnedMeshComponent::SetVisible(bool _visible)
{
	bRender = _visible;
	auto skinnedMesh = static_pointer_cast<USkeletalMeshResources>(m_pMesh);
	if (!bRender)
	{
		skinnedMesh->FreeInstanceIndex(m_instanceId);
		m_instanceId = -1;
	}
}
