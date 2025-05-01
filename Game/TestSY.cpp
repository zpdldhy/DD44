#include "pch.h"
#include "TestSY.h"
#include "UStaticMeshComponent.h"
#include "UMaterial.h"
#include "APawn.h"
#include "ACameraActor.h"
#include "ASky.h"
#include "EngineCameraMoveScript.h"
#include "kkongchiMoveScript.h"
#include "UTerrainMeshComponent.h"
#include "UMeshResources.h"
#include "CameraManager.h"
#include "Input.h"
#include <random>
#include "ATerrainTileActor.h"

void TestSY::Init()
{
	m_pCameraActor = make_shared<ACameraActor>();
	{
		m_pCameraActor->SetPosition({ 0.0f, 10.0f, 0.0f });
		m_pCameraActor->AddScript(make_shared<EngineCameraMoveScript>());

		m_pCameraActor->Init();
	}

	// HeightMap Test
	{
		//m_pTerrain = make_shared<ATerrainTileActor>();
		//m_pTerrain->m_fCellSize = 1.0f;

		//m_pTerrain->CreateTerrainFromHeightMap(
		//	L"../Resources/Texture/height.png",
		//	L"../Resources/Texture/kkongchi.jpg",
		//	L"../Resources/Shader/Default.hlsl"
		//);

		//m_pTerrain->Init();
	}

	{
		m_pTerrain = make_shared<ATerrainTileActor>();
		m_pTerrain->m_iNumCols = 20;
		m_pTerrain->m_iNumRows = 20;
		m_pTerrain->m_fCellSize = 10.0f;

		m_pTerrain->CreateTerrain(L"../Resources/Texture/grass.jpg", L"../Resources/Shader/Default.hlsl");

		auto vertexList = m_pTerrain->m_pTerrainMeshComponent->GetMesh()->GetVertexList();
		auto newVertexList = vertexList; // *참조가 아닌 복사인 상황

		std::default_random_engine eng((unsigned int)time(nullptr));
		std::uniform_int_distribution<int> distCol(1, m_pTerrain->m_iNumCols - 2);
		std::uniform_int_distribution<int> distRow(1, m_pTerrain->m_iNumRows - 2);
		std::uniform_real_distribution<float> heightDist(10.0f, 40.0f);

		int numHills = 5;
		for (int h = 0; h < numHills; ++h)
		{
			int col = distCol(eng);
			int row = distRow(eng);
			float centerHeight = heightDist(eng);

			int centerIdx = row * m_pTerrain->m_iNumCols + col;
			newVertexList[centerIdx].pos.y += centerHeight;

			for (int dz = -1; dz <= 1; ++dz)
			{
				for (int dx = -1; dx <= 1; ++dx)
				{
					int nx = col + dx;
					int nz = row + dz;
					if (nx < 0 || nx >= m_pTerrain->m_iNumCols || nz < 0 || nz >= m_pTerrain->m_iNumRows)
						continue;

					int nIdx = nz * m_pTerrain->m_iNumCols + nx;
					if (nIdx == centerIdx) continue;

					float dist = sqrtf((float)(dx * dx + dz * dz));
					float offset = std::max<float>(0.0f, centerHeight - dist * 10.0f);
					newVertexList[nIdx].pos.y += offset;
				}
			}
		}

		m_pTerrain->m_pTerrainMeshComponent->GetMesh()->SetVertexList(newVertexList);
		m_pTerrain->m_pTerrainMeshComponent->GetMesh()->Bind();

		m_pTerrain->Init();
	}

	{
		std::default_random_engine eng((unsigned int)time(nullptr));
		std::uniform_real_distribution<float> distX(-100.0f, 100.0f);
		std::uniform_real_distribution<float> distZ(-100.0f, 100.0f);
		std::uniform_real_distribution<float> scaleDist(3.0f, 8.0f);

		for (int i = 0; i < 30; ++i)
		{
			float x = distX(eng);
			float z = distZ(eng);
			float y = m_pTerrain->GetHeightAt(x, z);

			auto obj = make_shared<APawn>();
			auto mesh = UStaticMeshComponent::CreateCube();
			obj->SetMesh(mesh);
			obj->SetScale({ scaleDist(eng), scaleDist(eng), scaleDist(eng) });
			obj->SetPosition({ x, y + obj->GetScale().y/2, z});

			auto mat = make_shared<UMaterial>();
			mat->Load(L"../Resources/Texture/kkongchi.jpg", L"../Resources/Shader/Default.hlsl");
			mesh->SetMaterial(mat);

			obj->Init();
			m_vObjects.push_back(obj);
		}
	}

	{
		m_pActor = make_shared<APawn>();

		m_pStaticMesh = UStaticMeshComponent::CreateCube();
		m_pActor->SetMesh(m_pStaticMesh);
		m_pActor->SetScale({ 5.0f, 5.0f, 5.0f });
		m_pActor->SetPosition({ 0.0f, 2.5f, 10.0f });
		m_pActor->SetRotation({ 0.0f, 0.0f, 0.0f });

		shared_ptr<UMaterial> material = make_shared<UMaterial>();
		material->Load(L"../Resources/Texture/kkongchi.jpg", L"../Resources/Shader/Default.hlsl");
		m_pStaticMesh->SetMaterial(material);

		m_pActor->Init();
	}

	{
		m_pSky = make_shared<ASky>();

		m_pSkyMesh = UStaticMeshComponent::CreateSphere(20, 20);
		m_pSky->SetMeshComponent(m_pSkyMesh);

		shared_ptr<UMaterial> material = make_shared<UMaterial>();
		material->Load(L"../Resources/Texture/Sky.jpg", L"../Resources/Shader/Sky.hlsl");
		m_pSkyMesh->SetMaterial(material);

		m_pSky->Init();
	}

	m_pCameraActor->Init();
	m_pActor->Init();
	m_pPlane->Init();
	m_pSky->Init();

	CAMERAMANAGER->SetCameraActor(m_pCameraActor);
}

void TestSY::Update()
{
	CAMERAMANAGER->SetCameraActor(m_pCameraActor);
	if (INPUT->GetButtonDown(O))
		CAMERAMANAGER->SetCameraActor(m_pActor);

	m_pCameraActor->Tick();
	m_pActor->Tick();
	m_pTerrain->Tick();
	m_pSky->Tick();

	for (auto& obj : m_vObjects)
	{
		obj->Tick();
	}
}

void TestSY::Render()
{	
	if (INPUT->GetButtonDown(P))
		CAMERAMANAGER->Render(CameraViewType::CVT_UI);
	m_pCameraActor->Render();
	m_pActor->Render();
	m_pTerrain->Render();
	m_pSky->Render();

	for (auto& obj : m_vObjects)
	{
		obj->Render();
	}
}

void TestSY::Destroy()
{
}
