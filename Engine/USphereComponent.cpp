#include "pch.h"
#include "USphereComponent.h"
#include "DxState.h"
#include "AActor.h"
#include "UStaticMeshComponent.h"

void USphereComponent::Init()
{
	UShapeComponent::Init();
	UpdateBounds();

#ifndef DEBUG
	if (m_bVisible && g_bRangeVisibleMode)
	{
		CreateCollisionRange();
	}
#endif // DEBUG
}

void USphereComponent::Tick()
{
	UShapeComponent::Tick();
	UpdateBounds();

#ifndef DEBUG
	if (m_bVisible && g_bRangeVisibleMode)
	{
		UpdateCollisionRange();
		isRendered = false;
	}
#endif // DEBUG
}

void USphereComponent::Destroy()
{
#ifndef DEBUG
	if (m_pCollisionRange)
	{
		m_pCollisionRange->Destroy();
	}
#endif // DEBUG
}

void USphereComponent::UpdateBounds()
{
	m_Sphere.vCenter = m_vWorldPosition;
	m_Sphere.fRadius = 0.5f * m_vWorldScale.x;

	auto look = m_vLocalLook;	look.Normalize();
	auto down = -m_vLocalUp;	down.Normalize();

	m_LookRay.position = m_vWorldPosition;
	m_LookRay.direction = look * m_Sphere.fRadius;

	m_GroundRay.position = m_vWorldPosition;
	m_GroundRay.direction = down * m_Sphere.fRadius;
}

void USphereComponent::CreateCollisionRange()
{
	{
		m_pCollisionRange = make_shared<AActor>();

		auto pMesh = UStaticMeshComponent::CreateSphere(10, 10);
		m_pCollisionRange->SetMeshComponent(pMesh);

		auto pMaterial = make_shared<UMaterial>();
		pMaterial->Load(L"", L"../Resources/Shader/DefaultColor.hlsl");
		pMesh->SetMaterial(pMaterial);

		m_pCollisionRange->SetScale(Vec3(m_vWorldScale.x, m_vWorldScale.x, m_vWorldScale.x));
		m_pCollisionRange->SetRotation(m_vWorldRotation);
		m_pCollisionRange->SetPosition(m_vWorldPosition);

		m_pCollisionRange->Init();
		SetInstance(m_pCollisionRange);
	}
}

void USphereComponent::UpdateCollisionRange()
{
	m_pCollisionRange->SetScale(Vec3(m_vWorldScale.x, m_vWorldScale.x, m_vWorldScale.x));
	m_pCollisionRange->SetRotation(m_vWorldRotation);
	m_pCollisionRange->SetPosition(m_vWorldPosition);
	m_pCollisionRange->Tick();
}
