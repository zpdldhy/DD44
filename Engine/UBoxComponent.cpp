#include "pch.h"
#include "UBoxComponent.h"
#include "AActor.h"
#include "UMaterial.h"
#include "UStaticMeshComponent.h"
#include "Device.h"
#include "DxState.h"

array<shared_ptr<class AInstance>, static_cast<size_t>(RangeType::RT_COUNT)> m_pRangeInstance;

void UBoxComponent::Init()
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

void UBoxComponent::Tick()
{
	UShapeComponent::Tick();
	UpdateBounds();

#ifndef DEBUG
	if (m_bVisible && g_bRangeVisibleMode)
	{
		UpdateCollisionRange();
	}
#endif // DEBUG
}

void UBoxComponent::Destroy()
{
#ifndef DEBUG
	if (m_pCollisionRange)
	{
		m_pCollisionRange->Destroy();
		isRendered = false;
	}
#endif // DEBUG
}

void UBoxComponent::UpdateBounds()
{
	m_Box.vMin = Vec3::Transform(Vec3(-0.5f, -0.5f, -0.5f), m_matWorld);
	m_Box.vMax = Vec3::Transform(Vec3(+0.5f, +0.5f, +0.5f), m_matWorld);
	m_Box.vCenter = m_vWorldPosition;

	m_Box.vAxis[0] = m_vWorldRight;
	m_Box.vAxis[1] = m_vWorldUp;
	m_Box.vAxis[2] = m_vWorldLook;

	m_Box.vAxis[0].Normalize();
	m_Box.vAxis[1].Normalize();
	m_Box.vAxis[2].Normalize();	

	// 고정 박스라면 extents는 0.5, 0.5, 0.5
	m_Box.vExtent[0] = 0.5f * m_vWorldScale.x;
	m_Box.vExtent[1] = 0.5f * m_vWorldScale.y;
	m_Box.vExtent[2] = 0.5f * m_vWorldScale.z;

	m_LookRay.position = m_Box.vCenter;
	m_LookRay.direction = m_Box.vAxis[0] * m_Box.vExtent[0];

	m_GroundRay.position = m_Box.vCenter;
	m_GroundRay.direction = -m_Box.vAxis[1] * m_Box.vExtent[1];
}

void UBoxComponent::CreateCollisionRange()
{
	{
		m_pCollisionRange = make_shared<AActor>();

		auto pMesh = UStaticMeshComponent::CreateCube();
		m_pCollisionRange->SetMeshComponent(pMesh);

		auto pMaterial = make_shared<UMaterial>();
		pMaterial->Load(L"", L"../Resources/Shader/DefaultColor.hlsl");
		pMesh->SetMaterial(pMaterial);

		m_pCollisionRange->SetScale(m_vWorldScale);
		m_pCollisionRange->SetRotation(m_vWorldRotation);
		m_pCollisionRange->SetPosition(m_vWorldPosition);

		m_pCollisionRange->Init();
		SetInstance(m_pCollisionRange);
	}
	{
		m_pLookRange = make_shared<AActor>();

		auto pMesh = UStaticMeshComponent::CreateRay(Vec3(0.f, 0.f, 0.f), Vec3(0.f, 0.f, 0.5f));
		m_pLookRange->SetMeshComponent(pMesh);

		auto pMaterial = make_shared<UMaterial>();
		pMaterial->Load(L"", L"../Resources/Shader/DefaultColor.hlsl");		
		pMesh->SetMaterial(pMaterial);

		m_pLookRange->SetScale(m_vWorldScale);
		m_pLookRange->SetRotation(m_vWorldRotation);
		m_pLookRange->SetPosition(m_vWorldPosition);

		m_pLookRange->Init();
		SetInstance(m_pLookRange);
	}
	{
		m_pDownRange = make_shared<AActor>();

		auto pMesh = UStaticMeshComponent::CreateRay(Vec3(0.f, 0.f, 0.f), Vec3(0.f, -0.5f, 0.f));
		m_pDownRange->SetMeshComponent(pMesh);

		auto pMaterial = make_shared<UMaterial>();
		pMaterial->Load(L"", L"../Resources/Shader/DefaultColor.hlsl");
		pMesh->SetMaterial(pMaterial);

		m_pDownRange->SetScale(m_vWorldScale);
		m_pDownRange->SetRotation(m_vWorldRotation);
		m_pDownRange->SetPosition(m_vWorldPosition);

		m_pDownRange->Init();
		SetInstance(m_pDownRange);
	}
}

void UBoxComponent::UpdateCollisionRange()
{
	m_pCollisionRange->SetScale(m_vWorldScale);
	m_pCollisionRange->SetRotation(m_vWorldRotation);
	m_pCollisionRange->SetPosition(m_vWorldPosition);
	m_pCollisionRange->Tick();

	m_pLookRange->SetScale(m_vWorldScale);
	m_pLookRange->SetRotation(m_vWorldRotation);
	m_pLookRange->SetPosition(m_vWorldPosition);
	m_pLookRange->Tick();

	m_pDownRange->SetScale(m_vWorldScale);
	m_pDownRange->SetRotation(m_vWorldRotation);
	m_pDownRange->SetPosition(m_vWorldPosition);
	m_pDownRange->Tick();
}