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

#ifndef DEBUG
	if (m_bVisible && g_bRangeVisibleMode)
	{
		CreateCollisionRange();
	}
#endif // DEBUG
}

void UBoxComponent::Tick()
{
	USceneComponent::Tick();
	UpdateBounds();

#ifndef DEBUG
	if (m_bVisible && g_bRangeVisibleMode)
	{
		UpdateCollisionRange();
	}
#endif // DEBUG
}

void UBoxComponent::PreRender()
{
}

void UBoxComponent::Render()
{
#ifndef DEBUG
	if (m_bVisible && g_bRangeVisibleMode)
	{
		if (m_pCurrentRasterizer)
			m_pCurrentRasterizer.Reset();

		DC->RSGetState(m_pCurrentRasterizer.GetAddressOf());
		DC->RSSetState(STATE->m_pRSWireFrame.Get());

		m_pCollisionRange->Render();

		DC->RSSetState(m_pCurrentRasterizer.Get());

		m_pCurrentRasterizer.Reset();
	}
#endif // DEBUG
}

void UBoxComponent::PostRender()
{
}

void UBoxComponent::Destroy()
{
#ifndef DEBUG
	if (m_pCollisionRange)
	{
		m_pCollisionRange->Destroy();
	}
#endif // DEBUG
}

void UBoxComponent::UpdateBounds()
{
	Vec3 vMin(-0.5f, -0.5f, -0.5f);
	Vec3 vMax(+0.5f, +0.5f, +0.5f);
	Vec3 vCenter(0.f, 0.f, 0.f);

	Vec3 vAxisRoll(1.f, 0.f, 0.f);
	Vec3 vAxisYaw(0.f, 1.f, 0.f);
	Vec3 vAxisPitch(0.f, 0.f, 1.f);

	m_Box.vMin = Vec3::Transform(vMin, m_matWorld);
	m_Box.vMax = Vec3::Transform(vMax, m_matWorld);
	m_Box.vCenter = Vec3::Transform(vCenter, m_matWorld);

	m_Box.vAxis[0] = XMVector3TransformNormal(vAxisRoll, m_matWorld);
	m_Box.vAxis[1] = XMVector3TransformNormal(vAxisYaw, m_matWorld);
	m_Box.vAxis[2] = XMVector3TransformNormal(vAxisPitch, m_matWorld);

	m_Box.vAxis[0].Normalize();
	m_Box.vAxis[1].Normalize();
	m_Box.vAxis[2].Normalize();

	// 고정 박스라면 extents는 0.5, 0.5, 0.5
	m_Box.vExtent = Vec3(0.5f, 0.5f, 0.5f) * m_vWorldScale;
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
