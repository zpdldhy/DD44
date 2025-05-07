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
#include "MapEditorUI.h"
#include "UBoxComponent.h"
#include "ObjectLoader.h"
#include "ACharacter.h"

void TestSY::Init()
{
	m_pCameraActor = make_shared<ACameraActor>();
	{
		m_pCameraActor->SetPosition({ 0.0f, 10.0f, 0.0f });
		m_pCameraActor->AddScript(make_shared<EngineCameraMoveScript>());

		m_pCameraActor->Init();
		CAMERAMANAGER->SetCameraActor(m_pCameraActor);
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

		auto pCameraComponent = make_shared<UCameraComponent>();
		pCameraComponent->SetPosition(Vec3(20.f, 20.f, -20.f));
		pCameraComponent->SetNear(10.f);
		pCameraComponent->SetFar(100.f);
		m_pActor->SetCameraComponent(pCameraComponent);

		auto pBoxComponent = make_shared<UBoxComponent>();
		pBoxComponent->SetScale({ 6.f, 6.f, 6.f });
		m_pActor->SetShapeComponent(pBoxComponent);

		m_pActor->AddScript(make_shared<kkongchiMoveScript>());

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

	GUI->SetMapEditorCallback([this]()
	{
		MapEditorUI* editor = GUI->GetMapEditorUI();
		if (!editor) return;

		auto tile = std::make_shared<ATerrainTileActor>();

		tile->m_iNumCols = editor->GetNumCols();
		tile->m_iNumRows = editor->GetNumRows();
		tile->m_fCellSize = editor->GetCellSize();

		tile->CreateTerrain(editor->GetTexturePath(), editor->GetShaderPath());
		tile->SetPosition(editor->GetPosition());
		tile->SetRotation(editor->GetRotation());
		tile->SetScale(editor->GetScale());

		tile->Init();
		m_vTiles.push_back(tile);
	});

	GUI->SetObjectEditorCallback([this](int actorType, int meshType, const char* texPath, const char* shaderPath, const char* objPath, Vec3 pos, Vec3 rot, Vec3 scale)
	{
		AssimpLoader loader;
		vector<MeshData> meshList = loader.Load(objPath);
		if (meshList.empty())
			return;

		// Mesh Component ����
		shared_ptr<UMeshComponent> meshComp;
		if (meshType == 0)
		{
			// SkinnedMeshComponent ���� �� ���� Ȯ��
			meshComp = make_shared<UStaticMeshComponent>();

			auto meshRes = make_shared<UStaticMeshResources>();
			meshRes->SetVertexList(meshList[0].m_vVertexList);
			meshRes->SetIndexList(meshList[0].m_vIndexList);
			meshRes->Create();

			dynamic_cast<UStaticMeshComponent*>(meshComp.get())->SetMesh(meshRes);
		}
		else if (meshType == 1)
		{
			meshComp = make_shared<UStaticMeshComponent>();

			auto meshRes = make_shared<UStaticMeshResources>();
			meshRes->SetVertexList(meshList[0].m_vVertexList);
			meshRes->SetIndexList(meshList[0].m_vIndexList);
			meshRes->Create();

			dynamic_cast<UStaticMeshComponent*>(meshComp.get())->SetMesh(meshRes);
		}

		// ��Ƽ���� ����
		auto mat = make_shared<UMaterial>();
		mat->Load(
			std::wstring(texPath, texPath + strlen(texPath)),
			std::wstring(shaderPath, shaderPath + strlen(shaderPath))
		);
		meshComp->SetMaterial(mat);

		// Actor ����
		shared_ptr<AActor> actor;
		if (actorType == 0)
		{
			actor = make_shared<APawn>(); // ACharacter�� ��ü ����
		}
		else if (actorType == 1)
		{
			actor = make_shared<APawn>(); // AEnemy�� ��ü ����
		}
		else if (actorType == 2)
		{
			actor = make_shared<APawn>(); // AObject�� ��ü ����
		}

		actor->SetMeshComponent(meshComp);

		// Ÿ���� �ִٸ� ��ġ ����
		if (!m_vTiles.empty())
		{
			float y = m_vTiles.back()->GetHeightAt(pos.x, pos.z);
			pos.y = y + scale.y / 2.0f;
		}

		actor->SetPosition(pos);
		actor->SetRotation(rot);
		actor->SetScale(scale);
		actor->Init();

		m_vObjects.push_back(actor);
	});
}

void TestSY::Update()
{
	CAMERAMANAGER->SetCameraActor(m_pCameraActor);
	if (INPUT->GetButtonDown(O))
		CAMERAMANAGER->SetCameraActor(m_pActor);

	m_pCameraActor->Tick();
	m_pActor->Tick();
	m_pSky->Tick();

	for (auto& tile : m_vTiles)
		tile->Tick();
	
	for (auto& objects : m_vObjects)
		objects->Tick();
}

void TestSY::Render()
{	
	if (INPUT->GetButtonDown(P))
		CAMERAMANAGER->Render(CameraViewType::CVT_UI);
	
	m_pCameraActor->Render();
	m_pActor->Render();
	m_pSky->Render();

	for (auto& tile : m_vTiles)
		tile->Render();

	for (auto& objects : m_vObjects)
		objects->Render();
}

void TestSY::Destroy()
{
}
