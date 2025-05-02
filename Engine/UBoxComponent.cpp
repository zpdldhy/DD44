#include "pch.h"
#include "UBoxComponent.h"
#include "AActor.h"
#include "UMaterial.h"
#include "UStaticMeshComponent.h"

void UBoxComponent::Init()
{
	//m_vCenter = m_vLocalPosition + m_pOwner->GetPosition();

	//m_pRange = make_shared<AActor>();

	//auto pMesh = UStaticMeshComponent::CreateCube();
	//m_pRange->SetMeshComponent(pMesh);

	//auto pMaterial = make_shared<UMaterial>();
	//pMaterial->Load(L"", L"../Resources/Shader/DefaultColor.hlsl");
	//pMesh->SetMaterial(pMaterial);

	//m_pRange->SetScale(m_vBoxHalfSize);

	//m_pRange->Init();
}

void UBoxComponent::Tick()
{
	//m_vCenter = m_vLocalPosition + m_pOwner->GetPosition();
	//m_pRange->Tick();
}

void UBoxComponent::PreRender()
{
}

void UBoxComponent::Render()
{
	//m_pRange->Render();
}

void UBoxComponent::PostRender()
{
}

void UBoxComponent::Destroy()
{
	//m_pRange->Destroy();
}

void UBoxComponent::UpdateBounds()
{
}
