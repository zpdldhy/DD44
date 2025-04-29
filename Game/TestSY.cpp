#include "pch.h"
#include "TestSY.h"
#include "UStaticMeshComponent.h"
#include "UMaterial.h"
#include "APawn.h"
#include "ACameraActor.h"
#include "Input.h"
#include "Timer.h"
#include "ASky.h"

void TestSY::Init()
{
	m_pCameraActor = make_shared<ACameraActor>();
	{
		m_pCameraActor->SetPosition({ 0.0f, 0.0f, 10.0f });
	}

	{
		m_pActor = make_shared<AActor>();

		m_pStaticMesh = make_shared<UStaticMeshComponent>();
		m_pStaticMesh->CreateCube();
		m_pActor->SetMesh(m_pStaticMesh);
		m_pActor->SetScale({ 1.0f, 1.0f, 1.0f });
		m_pActor->SetPosition({ 0.0f, 0.0f, 0.0f });
		m_pActor->SetRotation({ 0.0f, 0.0f, 0.0f });

		shared_ptr<UMaterial> material = make_shared<UMaterial>();
		material->Load(L"../Resources/Texture/kkongchi.jpg", L"../Resources/Shader/Default.hlsl");
		m_pStaticMesh->SetMaterial(material);
	}

	{
		m_pSky = make_shared<ASky>();

		m_pSkyMesh = make_shared<UStaticMeshComponent>();
		m_pSkyMesh->CreateSphere(20, 20);
		m_pSky->SetMesh(m_pSkyMesh);

		shared_ptr<UMaterial> material = make_shared<UMaterial>();
		material->Load(L"../Resources/Texture/Sky.jpg", L"../Resources/Shader/Sky.hlsl");
		m_pSkyMesh->SetMaterial(material);
	}

	m_pCameraActor->Init();
	m_pActor->Init();
	m_pSky->Init();
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
	if (INPUT->GetButtonDown(Q))
	{
		m_pCameraActor->AddPosition(Vec3(0.0f, -1.0f, 0.0f) * deltaTime);
	}
	if (INPUT->GetButtonDown(E))
	{
		m_pCameraActor->AddPosition(Vec3(0.0f, 1.0f, 0.0f) * deltaTime);
	}


	m_pCameraActor->Tick();
	m_pActor->Tick();
	m_pSky->Tick();
}

void TestSY::Render()
{
	m_pCameraActor->Render();
	m_pActor->Render();
	m_pSky->Render();
}

void TestSY::Destroy()
{
}
