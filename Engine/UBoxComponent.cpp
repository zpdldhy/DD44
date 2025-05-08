#include "pch.h"
#include "UBoxComponent.h"
#include "AActor.h"
#include "UMaterial.h"
#include "UStaticMeshComponent.h"
#include "Device.h"
#include "DxState.h"

void UBoxComponent::Init()
{
	m_vCenter = m_vPosition + GetOwner()->GetPosition();

	CreateCollisionBox();
}

void UBoxComponent::Tick()
{
	m_vCenter = m_vPosition + m_pOwner.lock()->GetPosition();

	UpdateBounds();
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

void UBoxComponent::CreateCollisionBox()
{
	m_pCollisionRange = make_shared<AActor>();

	auto pMesh = UStaticMeshComponent::CreateCube();
	m_pCollisionRange->SetMeshComponent(pMesh);

	auto pMaterial = make_shared<UMaterial>();
	pMaterial->Load(L"", L"../Resources/Shader/DefaultColor.hlsl");
	pMesh->SetMaterial(pMaterial);

	m_pCollisionRange->SetScale(m_vScale);
	m_pCollisionRange->SetPosition(m_vCenter);

	m_pCollisionRange->Init();
}

void UBoxComponent::UpdateBounds()
{
	m_pCollisionRange->SetPosition(m_vCenter);
	m_pCollisionRange->Tick();
}
