#include "pch.h"
#include "TestSY.h"
#include "UStaticMeshComponent.h"
#include "UMaterial.h"
#include "APawn.h"
#include "ACameraActor.h"
#include "ASky.h"
#include "EngineCameraMoveScript.h"
#include "UTerrainMeshComponent.h"

void TestSY::Init()
{
	m_pCameraActor = make_shared<ACameraActor>();
	{
		m_pCameraActor->SetPosition({ 0.0f, 0.0f, 0.0f });
		m_pCameraActor->AddScript(make_shared<EngineCameraMoveScript>());
	}

	{
		m_pActor = make_shared<APawn>();

		m_pStaticMesh = make_shared<UStaticMeshComponent>();
		m_pStaticMesh->CreateCube();
		m_pActor->SetMesh(m_pStaticMesh);
		m_pActor->SetScale({ 1.0f, 1.0f, 1.0f });
		m_pActor->SetPosition({ 0.0f, 0.0f, 10.0f });
		m_pActor->SetRotation({ 0.0f, 0.0f, 0.0f });

		shared_ptr<UMaterial> material = make_shared<UMaterial>();
		material->Load(L"../Resources/Texture/kkongchi.jpg", L"../Resources/Shader/Default.hlsl");
		m_pStaticMesh->SetMaterial(material);
	}

	{
		DirectX::ScratchImage heightImage;
		DirectX::LoadFromWICFile(L"../Resources/Texture/height.png", DirectX::WIC_FLAGS_FORCE_RGB, nullptr, heightImage);
		const DirectX::Image* img = heightImage.GetImage(0, 0, 0);
		UINT8* pixels = img->pixels;
		int width = static_cast<int>(img->width);
		int height = static_cast<int>(img->height);

		m_pPlane = make_shared<APawn>();

		m_pPlaneMesh = make_shared<UTerrainMeshComponent>();
		m_pPlaneMesh->CreateGrid(width, height, 1.0f);
		for (int y = 0; y < height; ++y)
		{
			for (int x = 0; x < width; ++x)
			{
				int idx = y * width + x;
				UINT8 gray = pixels[y * img->rowPitch + x];
				float heightValue = (gray / 255.0f * 500.0f) - 300.0f;
				m_pPlaneMesh->m_vVertexList[idx].pos.y = heightValue;
			}
		}
		m_pPlaneMesh->CreateVertexBuffer();
		m_pPlane->SetMesh(m_pPlaneMesh);

		shared_ptr<UMaterial> material = make_shared<UMaterial>();
		material->Load(L"../Resources/Texture/kkongchi.jpg", L"../Resources/Shader/Default.hlsl");
		m_pPlaneMesh->SetMaterial(material);
	}

	{
		//m_pTexture = TEXTURE->Load(L"../Resources/Texture/height.png");
		//int width = m_pTexture->width;
		//int height = m_pTexture->height;

		//m_pPlane = make_shared<AActor>();

		//m_pPlaneMesh = make_shared<UTerrainMeshComponent>();
		//m_pPlaneMesh->CreateGrid(width, height, 10.0f);
		//m_pPlane->SetMesh(m_pPlaneMesh);
		//m_pPlane->SetScale({ 10.0f, 10.0f, 10.0f });
		//m_pPlane->SetPosition({ 0.0f, 0.0f, 0.0f });
		//m_pPlane->SetRotation({ 0.0f, 0.0f, 0.0f });

		//shared_ptr<UMaterial> material = make_shared<UMaterial>();
		//material->Load(L"../Resources/Texture/kkongchi.jpg", L"../Resources/Shader/Default.hlsl");
		//m_pPlaneMesh->SetMaterial(material);
	}

	{
		m_pSky = make_shared<ASky>();

		m_pSkyMesh = make_shared<UTerrainMeshComponent>();
		m_pSkyMesh->CreateSphere(20, 20);
		m_pSky->SetMesh(m_pSkyMesh);

		shared_ptr<UMaterial> material = make_shared<UMaterial>();
		material->Load(L"../Resources/Texture/Sky.jpg", L"../Resources/Shader/Sky.hlsl");
		m_pSkyMesh->SetMaterial(material);
	}

	m_pCameraActor->Init();
	m_pActor->Init();
	m_pPlane->Init();
	m_pSky->Init();
}

void TestSY::Update()
{
	m_pCameraActor->Tick();
	m_pActor->Tick();
	m_pPlane->Tick();
	m_pSky->Tick();
}

void TestSY::Render()
{
	m_pCameraActor->Render();
	m_pActor->Render();
	m_pPlane->Render();
	m_pSky->Render();
}

void TestSY::Destroy()
{
}
