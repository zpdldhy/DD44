#include "pch.h"
#include "UBoxComponent.h"
#include "AActor.h"
#include "UMaterial.h"
#include "UStaticMeshComponent.h"
#include "Device.h"
#include "DxState.h"

void UBoxComponent::Init()
{
	USceneComponent::Init();
	UpdateBounds();

	if (m_bVisible)
	{
		CreateCollisionRange();
	}

}

void UBoxComponent::Tick()
{
	USceneComponent::Tick();
	UpdateBounds();

	if (m_bVisible)
	{
		UpdateCollisionRange();
	}
}

void UBoxComponent::PreRender()
{
}

void UBoxComponent::Render()
{
	if (m_bVisible)
	{
		if (m_pCurrentRasterizer)
			m_pCurrentRasterizer.Reset();

		DC->RSGetState(m_pCurrentRasterizer.GetAddressOf());
		DC->RSSetState(STATE->m_pRSWireFrame.Get());

		m_pCollisionRange->Render();

		DC->RSSetState(m_pCurrentRasterizer.Get());

		m_pCurrentRasterizer.Reset();
	}
}

void UBoxComponent::PostRender()
{
}

void UBoxComponent::Destroy()
{
	if (m_pCollisionRange)
	{
		m_pCollisionRange->Destroy();
	}
}

void UBoxComponent::UpdateBounds()
{
	Vec3 vMin(-0.5f, -0.5f, -0.5f);
	Vec3 vMax(+0.5f, +0.5f, +0.5f);
	Vec3 vCenter(0.f, 0.f, 0.f);

	m_Box.vMin = Vec3::Transform(vMin, m_matWorld);
	m_Box.vMax = Vec3::Transform(vMax, m_matWorld);
	m_Box.vCenter = Vec3::Transform(vCenter, m_matWorld);
}

void UBoxComponent::CreateCollisionRange()
{
	m_pCollisionRange = make_shared<AActor>();

	auto pMesh = UStaticMeshComponent::CreateCube();
	m_pCollisionRange->SetMeshComponent(pMesh);

	auto pMaterial = make_shared<UMaterial>();
	pMaterial->Load(L"", L"../Resources/Shader/DefaultColor.hlsl");
	pMesh->SetMaterial(pMaterial);

	m_pCollisionRange->SetScale(m_vLocalScale);
	m_pCollisionRange->SetRotation(m_vWorldRotation);
	m_pCollisionRange->SetPosition(m_vWorldPosition);

	m_pCollisionRange->Init();
}

void UBoxComponent::UpdateCollisionRange()
{
	m_pCollisionRange->SetScale(m_vWorldScale);
	m_pCollisionRange->SetRotation(m_vWorldRotation);
	m_pCollisionRange->SetPosition(m_vWorldPosition);
	m_pCollisionRange->Tick();
}
