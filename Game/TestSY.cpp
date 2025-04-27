#include "pch.h"
#include "TestSY.h"
#include "UStaticMeshComponent.h"
#include "UMaterial.h"
#include "ACameraActor.h"
#include "Input.h"
#include "Timer.h"

void TestSY::Init()
{
	m_pCameraActor = make_shared<ACameraActor>();
	{
		m_pCameraActor->SetPosition({ 0.0f, 0.0f, 10.0f });
	}

	m_pActor = make_shared<AActor>();

	m_pStaticMesh = make_shared<UStaticMeshComponent>();
	m_pStaticMesh->CreateCube();
	shared_ptr<UMaterial> material = make_shared<UMaterial>();
	material->Load(L"../Resources/Texture/kkongchi.jpg", L"../Resources/Shader/Default.hlsl");
	m_pStaticMesh->SetMaterial(material);

	m_pActor->SetMesh(m_pStaticMesh);
	m_pActor->SetScale({ 1.0f, 1.0f, 1.0f });
	m_pActor->SetPosition({ 0.0f, 0.0f, -10.0f });
	m_pActor->SetRotation({ 0.0f, 0.0f, 0.0f });

	m_pCameraActor->Init();
	m_pActor->Init();
}

void TestSY::Update()
{
	float deltaTime = TIMER->GetDeltaTime() * 10.f;

	if (INPUT->GetButtonDown(W))
	{
		m_pCameraActor->AddPosition(Vec3(0.0f, 0.0f, -1.0f) * deltaTime);
	}
	if (INPUT->GetButtonDown(A))
	{
		m_pCameraActor->AddPosition(Vec3(-1.0f, 0.0f, 0.0f) * deltaTime);
	}
	if (INPUT->GetButtonDown(S))
	{
		m_pCameraActor->AddPosition(Vec3(0.0f, 0.0f, 1.0f) * deltaTime);
	}
	if (INPUT->GetButtonDown(D))
	{
		m_pCameraActor->AddPosition(Vec3(1.0f, 0.0f, 0.0f) * deltaTime);
	}		


	m_pCameraActor->Tick();
	m_pActor->Tick();
}

void TestSY::Render()
{
	m_pCameraActor->Render();
	m_pActor->Render();
}

void TestSY::Destroy()
{
}
