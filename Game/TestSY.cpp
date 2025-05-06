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

	{
		//AssimpLoader loader;
		//vector<MeshData> meshList = loader.Load("../Resources/Obj/buggy_floor.obj");
		//if (meshList.empty())
		//{
		//	return;
		//}

		//auto meshRes = make_shared<UStaticMeshResources>();
		//meshRes->SetVertexList(meshList[0].m_vVertexList); 
		//meshRes->SetIndexList(meshList[0].m_vIndexList);
		//meshRes->Create();

		//m_pObjMesh = make_shared<UStaticMeshComponent>();
		//m_pObjMesh->SetMesh(meshRes);

		//auto material = make_shared<UMaterial>();
		//material->Load(L"../Resources/Texture/buggyStationTexture.png", L"../Resources/Shader/Default.hlsl");
		//m_pObjMesh->SetMaterial(material);

		//m_pObj = make_shared<APawn>();
		//m_pObj->SetMeshComponent(m_pObjMesh);

		//Vec3 pos = Vec3(30.0f, 0.0f, 30.0f);
		//if (!m_vTiles.empty())
		//{
		//	auto tile = m_vTiles.back();
		//	pos.y = tile->GetHeightAt(pos.x, pos.z);
		//}

		//m_pObj->SetPosition(pos);
		//m_pObj->SetScale({ 3.0f, 3.0f, 3.0f });
		//m_pObj->Init();
	}
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

	if (ImGui::Begin("Object Placement Tool"))
	{
		static const char* actorTypes[] = { "Character", "Enemy", "Object"};
		static int currentActorType = 0;

		static const char* meshTypes[] = { "Skinned", "Static", "Terrain" };
		static int currentMeshType = 0;

		static float position[3] = { 0.0f, 0.0f, 0.0f };
		static float rotation[3] = { 0.0f, 0.0f, 0.0f };
		static float scale[3] = { 1.0f, 1.0f, 1.0f };

		static char texturePath[256] = "../Resources/Texture/kkongchi.jpg";
		static char shaderPath[256] = "../Resources/Shader/Default.hlsl";
		static char ObjPath[256] = "../Resources/Obj/buggy_floor.obj";

		ImGui::Text("Actor Type");
		ImGui::Combo("##ActorType", &currentActorType, actorTypes, IM_ARRAYSIZE(actorTypes));

		ImGui::Text("Mesh");
		ImGui::Combo("##MeshCombo", &currentMeshType, meshTypes, IM_ARRAYSIZE(meshTypes));

		ImGui::InputFloat3("Position", position);
		ImGui::InputFloat3("Rotation", rotation);
		ImGui::InputFloat3("Scale", scale);

		ImGui::InputText("Texture Path", texturePath, IM_ARRAYSIZE(texturePath));
		ImGui::InputText("Shader Path", shaderPath, IM_ARRAYSIZE(shaderPath));
		ImGui::InputText("Obj Path", ObjPath, IM_ARRAYSIZE(ObjPath));

		if (ImGui::Button("Create"))
		{
			AssimpLoader loader;
			vector<MeshData> meshList = loader.Load(ObjPath);

			if (!meshList.empty())
			{
				shared_ptr<UMeshComponent> meshComp;
				if (currentMeshType == 0)
				{

				}
				else if (currentMeshType == 1)
				{
					meshComp = make_shared<UStaticMeshComponent>();

					auto meshRes = make_shared<UStaticMeshResources>();
					meshRes->SetVertexList(meshList[0].m_vVertexList);
					meshRes->SetIndexList(meshList[0].m_vIndexList);
					meshRes->Create();

					dynamic_cast<UStaticMeshComponent*>(meshComp.get())->SetMesh(meshRes);
				}
				else if (currentMeshType == 2)
				{

				}

				//auto meshComp = make_shared<UStaticMeshComponent>();

				//auto meshRes = make_shared<UStaticMeshResources>();
				//meshRes->SetVertexList(meshList[0].m_vVertexList);
				//meshRes->SetIndexList(meshList[0].m_vIndexList);
				//meshRes->Create();

				//meshComp->SetMesh(meshRes);

				auto mat = make_shared<UMaterial>();
				mat->Load(
					std::wstring(texturePath, texturePath + strlen(texturePath)),
					std::wstring(shaderPath, shaderPath + strlen(shaderPath))
				);
				meshComp->SetMaterial(mat);

				shared_ptr<AActor> actor;
				if (currentActorType == 0)
				{
					//actor = make_shared<ACharacter>();
				}
				else if (currentActorType == 1)
				{
					actor = make_shared<APawn>();
				}
				else if (currentActorType == 2)
				{
					actor = make_shared<APawn>();
				}
					
				//auto actor = make_shared<APawn>();
				actor->SetMeshComponent(meshComp);
				Vec3 pos(position[0], position[1], position[2]);

				actor->SetPosition(pos);
				actor->SetRotation(Vec3(rotation[0], rotation[1], rotation[2]));
				actor->SetScale(Vec3(scale[0], scale[1], scale[2]));
				actor->Init();

				m_vObjects.push_back(actor);
			}
		}
	}
	ImGui::End();

	for (auto& objects : m_vObjects)
		objects->Tick();
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
	m_pSky->Render();

	for (auto& tile : m_vTiles)
		tile->Render();

	for (auto& objects : m_vObjects)
		objects->Render();
	
}

void TestSY::Destroy()
{
}
