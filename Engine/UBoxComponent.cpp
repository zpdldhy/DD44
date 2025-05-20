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
	CreateCollisionRange();
}

void UBoxComponent::Tick()
{
	USceneComponent::Tick();
	UpdateCollisionRange();
}

void UBoxComponent::PreRender()
{
}

void UBoxComponent::Render()
{
	if (m_pCurrentRasterizer)
		m_pCurrentRasterizer.Reset();

	DC->RSGetState(m_pCurrentRasterizer.GetAddressOf());
	DC->RSSetState(STATE->m_pRSWireFrame.Get());

	m_pCollisionRange->Render();

	DC->RSSetState(m_pCurrentRasterizer.Get());

	m_pCurrentRasterizer.Reset();
}

void UBoxComponent::PostRender()
{
}

void UBoxComponent::Destroy()
{
	m_pCollisionRange->Destroy();
}

void UBoxComponent::UpdateBounds()
{
	Vec3 vMin(-0.5f, -0.5f, -0.5f);
	Vec3 vMax(+0.5f, +0.5f, +0.5f);

	vMin *= m_vLocalScale;
	vMax *= m_vLocalScale;

	m_Box.Set(vMin, vMax);
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
	m_pCollisionRange->SetPosition(m_Box.vCenter);

	m_pCollisionRange->Init();
}

void UBoxComponent::UpdateCollisionRange()
{
	m_pCollisionRange->SetPosition(m_vWorldPosition);
	m_pCollisionRange->Tick();
}
