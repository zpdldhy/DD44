#include "pch.h"
#include "TestSY.h"
#include "UStaticMeshComponent.h"
#include "UMaterial.h"
#include "AActor.h"

void TestSY::Init()
{
	m_pStaticMesh = make_shared<UStaticMeshComponent>();
	m_pStaticMesh->CreateCube();
	shared_ptr<UMaterial> material = make_shared<UMaterial>();
	material->Load(L"../Resources/Texture/kkongchi.jpg", L"../Resources/Shader/Default.hlsl");
	m_pStaticMesh->SetMaterial(material);

	m_pActor = make_shared<AActor>();
	m_pActor->SetMesh(m_pStaticMesh);
	m_pActor->SetScale({ 1.0f, 1.0f, 1.0f });
	m_pActor->SetPosition({ 0.0f, 0.0f, 0.0f });
	m_pActor->SetRotation({ 0.0f, 0.0f, 0.0f });

	m_pActor->Init();
}

void TestSY::Update()
{
	m_pActor->Tick();
}

void TestSY::Render()
{
	m_pActor->Render();
}

void TestSY::Destroy()
{
}
