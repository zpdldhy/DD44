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
#include "ImGuiCore.h"
#include "DxState.h"

void TestSY::Init()
{
	m_pCameraActor = make_shared<ACameraActor>();
	{
		m_pCameraActor->SetPosition({ 0.0f, 10.0f, 0.0f });
		m_pCameraActor->AddScript(make_shared<EngineCameraMoveScript>());

		m_pCameraActor->Init();
		CAMERAMANAGER->SetCameraActor(m_pCameraActor);
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

	// ���� Ÿ�ϸ�
	{
		//m_pTerrain = make_shared<ATerrainTileActor>();
		//m_pTerrain->m_iNumCols = 20;
		//m_pTerrain->m_iNumRows = 20;
		//m_pTerrain->m_fCellSize = 10.0f;

		//m_pTerrain->CreateTerrain(L"../Resources/Texture/grass.jpg", L"../Resources/Shader/Default.hlsl");

		//auto vertexList = m_pTerrain->m_pTerrainMeshComponent->GetMesh()->GetVertexList();
		//auto newVertexList = vertexList; // *������ �ƴ� ������ ��Ȳ

		//std::default_random_engine eng((unsigned int)time(nullptr));
		//std::uniform_int_distribution<int> distCol(1, m_pTerrain->m_iNumCols - 2);
		//std::uniform_int_distribution<int> distRow(1, m_pTerrain->m_iNumRows - 2);
		//std::uniform_real_distribution<float> heightDist(10.0f, 40.0f);

		//int numHills = 5;
		//for (int h = 0; h < numHills; ++h)
		//{
		//	int col = distCol(eng);
		//	int row = distRow(eng);
		//	float centerHeight = heightDist(eng);

		//	int centerIdx = row * m_pTerrain->m_iNumCols + col;
		//	newVertexList[centerIdx].pos.y += centerHeight;

		//	for (int dz = -1; dz <= 1; ++dz)
		//	{
		//		for (int dx = -1; dx <= 1; ++dx)
		//		{
		//			int nx = col + dx;
		//			int nz = row + dz;
		//			if (nx < 0 || nx >= m_pTerrain->m_iNumCols || nz < 0 || nz >= m_pTerrain->m_iNumRows)
		//				continue;

		//			int nIdx = nz * m_pTerrain->m_iNumCols + nx;
		//			if (nIdx == centerIdx) continue;

		//			float dist = sqrtf((float)(dx * dx + dz * dz));
		//			float offset = std::max<float>(0.0f, centerHeight - dist * 10.0f);
		//			newVertexList[nIdx].pos.y += offset;
		//		}
		//	}
		//}

		//m_pTerrain->m_pTerrainMeshComponent->GetMesh()->SetVertexList(newVertexList);
		//m_pTerrain->m_pTerrainMeshComponent->GetMesh()->Bind();

		//m_pTerrain->Init();
	}

	// ���� ������Ʈ
	{
		//std::default_random_engine eng((unsigned int)time(nullptr));
		//std::uniform_real_distribution<float> distX(-100.0f, 100.0f);
		//std::uniform_real_distribution<float> distZ(-100.0f, 100.0f);
		//std::uniform_real_distribution<float> scaleDist(3.0f, 8.0f);

		//for (int i = 0; i < 30; ++i)
		//{
		//	float x = distX(eng);
		//	float z = distZ(eng);
		//	float y = m_pTerrain->GetHeightAt(x, z);

		//	auto obj = make_shared<APawn>();
		//	auto mesh = UStaticMeshComponent::CreateCube();
		//	obj->SetMeshComponent(mesh);
		//	obj->SetScale({ scaleDist(eng), scaleDist(eng), scaleDist(eng) });
		//	obj->SetPosition({ x, y + obj->GetScale().y/2, z});

		//	auto mat = make_shared<UMaterial>();
		//	mat->Load(L"../Resources/Texture/kkongchi.jpg", L"../Resources/Shader/Default.hlsl");
		//	mesh->SetMaterial(mat);

		//	obj->Init();
		//	m_vObjects.push_back(obj);
		//}
	}

	{
		m_pActor = make_shared<APawn>();

		m_pStaticMesh = UStaticMeshComponent::CreateCube();
		m_pActor->SetMeshComponent(m_pStaticMesh);
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

	
}

void TestSY::Update()
{
	CAMERAMANAGER->SetCameraActor(m_pCameraActor);
	if (INPUT->GetButtonDown(O))
		CAMERAMANAGER->SetCameraActor(m_pActor);

	m_pCameraActor->Tick();
	m_pActor->Tick();
	//m_pTerrain->Tick();
	m_pSky->Tick();

	// Map tool
	DrawTerrainEditorUI();

	for (auto& tile : m_vTiles)
	{
		tile->Tick();
	}
	for (auto& obj : m_vObjects)
	{
		obj->Tick();
	}
}

void TestSY::Render()
{	
	if (m_bEditorWireframe)
	{
		DC->RSSetState(STATE->m_pRSWireFrame.Get());
	}
	else
	{
		DC->RSSetState(STATE->m_pRSSolid.Get());
	}

	if (INPUT->GetButtonDown(P))
		CAMERAMANAGER->Render(CameraViewType::CVT_UI);
	m_pCameraActor->Render();
	m_pActor->Render();
	//m_pTerrain->Render();


	for (auto& tile : m_vTiles)
	{
		tile->Render();
	}
	for (auto& obj : m_vObjects)
	{
		obj->Render();
	}

	m_pSky->Render();
}

void TestSY::Destroy()
{
}

void TestSY::DrawTerrainEditorUI()
{
	static int numCols = 20;
	static int numRows = 20;
	static float cellSize = 10.0f;

	static float position[3] = { 0.0f, 0.0f, 0.0f };
	static float rotation[3] = { 0.0f, 0.0f, 0.0f };
	static float scale[3] = { 1.0f, 1.0f, 1.0f };

	static char texturePath[256] = "../Resources/Texture/grass.jpg";
	static char shaderPath[256] = "../Resources/Shader/Default.hlsl";

	ImGui::Begin("Terrain Placement");

	// Section: Terrain Configuration
	ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), "Terrain Configuration");
	ImGui::InputInt("Num Cols", &numCols);
	ImGui::InputInt("Num Rows", &numRows);
	ImGui::InputFloat("Cell Size", &cellSize);
	ImGui::Separator(); ImGui::Spacing();

	// Section: Transform (SRT)
	ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), "Transform");
	ImGui::InputFloat3("Position", position);
	ImGui::InputFloat3("Rotation", rotation);
	ImGui::InputFloat3("Scale", scale);
	ImGui::Separator(); ImGui::Spacing();

	// Section: Appearance
	ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), "Appearance");
	ImGui::InputText("Texture Path", texturePath, IM_ARRAYSIZE(texturePath));
	ImGui::InputText("Shader Path", shaderPath, IM_ARRAYSIZE(shaderPath));
	ImGui::Spacing(); ImGui::Spacing();

	// Action Button
	if (ImGui::Button("Create Terrain Tile", ImVec2(-1, 0)))  // -1 = full width
	{
		auto tile = make_shared<ATerrainTileActor>();
		tile->m_iNumCols = numCols;
		tile->m_iNumRows = numRows;
		tile->m_fCellSize = cellSize;

		tile->CreateTerrain(
			std::wstring(texturePath, texturePath + strlen(texturePath)),
			std::wstring(shaderPath, shaderPath + strlen(shaderPath))
		);

		tile->SetPosition(Vec3(position[0], position[1], position[2]));
		tile->SetRotation(Vec3(rotation[0], rotation[1], rotation[2]));
		tile->SetScale(Vec3(scale[0], scale[1], scale[2]));

		tile->Init();
		m_vTiles.push_back(tile);
	}

	// Debug Options
	ImGui::Separator(); ImGui::Spacing();
	ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), "Debug Options");
	ImGui::Checkbox("Wireframe Mode", &m_bEditorWireframe);

	ImGui::End();

}
