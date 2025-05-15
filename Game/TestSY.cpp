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
#include "AUIActor.h"
#include "UIManager.h"
#include "ObjectLoader.h"
#include "ACharacter.h"
#include "ObjectManager.h"
#include "ActorLoader.h"
#include "PlayerMoveScript.h"
#include "UAnimInstance.h"
#include "USkinnedMeshComponent.h"
#include "ActorLoader.h"
#include "USkeletalMeshResources.h"
#include "PrefabLoader.h"
#include "Timer.h"
#include "Functions.h"
#include "CollisionManager.h"

void TestSY::Init()
{
	m_pCameraActor = make_shared<ACameraActor>();
	{
		m_pCameraActor->SetPosition({ 0.0f, 10.0f, 0.0f });
		m_pCameraActor->AddScript(make_shared<EngineCameraMoveScript>());
		m_pCameraActor->m_szName = L"EnginCamera";

		CAMERA->Set3DCameraActor(m_pCameraActor);
	}

	//LoadAllPrefabs(".map.json");

	{
		m_pActor = make_shared<APawn>();
		m_pActor->m_szName = L"kkongchi";

		m_pStaticMesh = UStaticMeshComponent::CreateCube();
		m_pActor->SetMeshComponent(m_pStaticMesh);
		m_pActor->SetScale({ 5.0f, 5.0f, 5.0f });
		m_pActor->SetPosition({ 0.0f, 0.0f, 0.0f });
		m_pActor->SetRotation({ 0.0f, 0.0f, 0.0f });

		shared_ptr<UMaterial> material = make_shared<UMaterial>();
		material->Load(L"../Resources/Texture/kkongchi.jpg", L"../Resources/Shader/Effect.hlsl");
		m_pStaticMesh->SetMaterial(material);

		auto pCameraComponent = make_shared<UCameraComponent>();
		pCameraComponent->SetLocalPosition(Vec3(20.f, 0.f, -0.f));
		pCameraComponent->SetNear(10.f);
		pCameraComponent->SetFar(100.f);
		m_pActor->SetCameraComponent(pCameraComponent);

		auto pBoxComponent = make_shared<UBoxComponent>();
		pBoxComponent->SetLocalScale({ 6.f, 6.f, 6.f });
		pBoxComponent->SetCollisionEnabled(CollisionEnabled::CE_QUERYONLY);
		m_pActor->SetShapeComponent(pBoxComponent);

		m_pActor->AddScript(make_shared<kkongchiMoveScript>());
		m_pActor->m_bCollision = true;
	}

	{
		auto pActor = make_shared<APawn>();
		pActor->SetPosition({ 20.f, 0.f, 0.f });

		auto pMesh = UStaticMeshComponent::CreateTriangle();
		pActor->SetMeshComponent(pMesh);

		auto pMaterial = make_shared<UMaterial>();
		pMaterial->Load(L"", L"../Resources/Shader/DefaultColor.hlsl");
		pMesh->SetMaterial(pMaterial);

		auto pBoxComponent = make_shared<UBoxComponent>();
		pBoxComponent->SetLocalScale({ 6.f, 6.f, 6.f });
		pBoxComponent->SetCollisionEnabled(CollisionEnabled::CE_QUERYONLY);
		pActor->SetShapeComponent(pBoxComponent);

		pActor->m_bCollision = true;

		OBJECT->AddActor(pActor);
	}

	{
		m_pSky = make_shared<ASky>();
		m_pSky->m_szName = L"Sky";

		m_pSkyMesh = UStaticMeshComponent::CreateSphere(20, 20);
		m_pSky->SetMeshComponent(m_pSkyMesh);

		shared_ptr<UMaterial> material = make_shared<UMaterial>();
		material->Load(L"../Resources/Texture/Sky.jpg", L"../Resources/Shader/Sky.hlsl");
		m_pSkyMesh->SetMaterial(material);
	}

	// UIs
	{
		// Ability
		{
			auto pUIArrowBack = make_shared<AUIActor>();
			pUIArrowBack->SetScale(Vec3(169.f, 166.f, 0.f) * 0.6f);
			pUIArrowBack->SetPosition(Vec3(-600.f, 370.f, 0.9f));

			{
				auto pMesh = UStaticMeshComponent::CreatePlane();
				pUIArrowBack->SetMeshComponent(pMesh);

				auto pMaterial = make_shared<UMaterial>();
				pMaterial->Load(L"../Resources/Texture/hud_abilty_box.png", L"../Resources/Shader/Default.hlsl");
				pMesh->SetMaterial(pMaterial);
			}

			UI->AddUI(pUIArrowBack);

			auto pUIArrowFrame = make_shared<AUIActor>();
			pUIArrowFrame->SetScale(Vec3(182.f, 181.f, 0.f) * 0.6f);
			pUIArrowFrame->SetPosition(pUIArrowBack->GetPosition() + Vec3(0.f, 0.f, -0.1f));

			{
				auto pMesh = UStaticMeshComponent::CreatePlane();
				pUIArrowFrame->SetMeshComponent(pMesh);

				auto pMaterial = make_shared<UMaterial>();
				pMaterial->Load(L"../Resources/Texture/hud_abilty_frame_chipped.png", L"../Resources/Shader/Default.hlsl");
				pMesh->SetMaterial(pMaterial);
			}

			UI->AddUI(pUIArrowFrame);

			auto pUIArrow = make_shared<AUIActor>();
			pUIArrow->SetScale(Vec3(107.f, 108.f, 0.f) * 0.4f);
			pUIArrow->SetPosition(pUIArrowBack->GetPosition() + Vec3(-5.f, -5.f, -0.1f));

			{
				auto pMesh = UStaticMeshComponent::CreatePlane();
				pUIArrow->SetMeshComponent(pMesh);

				auto pMaterial = make_shared<UMaterial>();
				pMaterial->Load(L"../Resources/Texture/Icon_Arrow.png", L"../Resources/Shader/Default.hlsl");
				pMesh->SetMaterial(pMaterial);
			}

			UI->AddUI(pUIArrow);
		}

		// Energe
		{
			auto pUIEnergyBack = make_shared<AUIActor>();
			pUIEnergyBack->SetScale(Vec3(64.f, 64.f, 0.f) * 0.5f);
			pUIEnergyBack->SetPosition(Vec3(-500.f, 360.f, 0.9f));

			{
				auto pMesh = UStaticMeshComponent::CreatePlane();
				pUIEnergyBack->SetMeshComponent(pMesh);

				auto pMaterial = make_shared<UMaterial>();
				pMaterial->Load(L"../Resources/Texture/MP_Box.png", L"../Resources/Shader/Default.hlsl");
				pMesh->SetMaterial(pMaterial);
			}

			UI->AddUI(pUIEnergyBack);

			auto pUIEnergy = make_shared<AUIActor>();
			pUIEnergy->SetScale(Vec3(64.f, 64.f, 0.f) * 0.5f);
			pUIEnergy->SetPosition(pUIEnergyBack->GetPosition() + Vec3(0.f, 0.f, -0.1f));

			{
				auto pMesh = UStaticMeshComponent::CreatePlane();
				pUIEnergy->SetMeshComponent(pMesh);

				auto pMaterial = make_shared<UMaterial>();
				pMaterial->Load(L"../Resources/Texture/MP_Blip.png", L"../Resources/Shader/Default.hlsl");
				pMesh->SetMaterial(pMaterial);
			}

			UI->AddUI(pUIEnergy);
		}
	}

	GUI->SetCharacterEditorCallback(
		[](std::shared_ptr<UMeshComponent> rootComponent, const Vec3& position, const Vec3& rotation, const Vec3& scale, int scriptType)
		{
			if (!rootComponent)
			{
				return;
			}

			auto actor = std::make_shared<AActor>();
			//actor->SetActorName(rootComponent->GetName());
			actor->m_szName = L"Character";

			actor->SetMeshComponent(rootComponent);

			actor->SetPosition(position);
			actor->SetRotation(rotation);
			actor->SetScale(scale);

			if (scriptType == 1) actor->AddScript(std::make_shared<PlayerMoveScript>());
			//if (scriptType == 2) actor->AddScript(std::make_shared<EnemyAIScript>());

			auto cam = std::make_shared<UCameraComponent>();
			cam->SetLocalPosition(Vec3(10, 10, -10));
			actor->SetCameraComponent(cam);

			OBJECT->AddActor(actor);
		});

	GUI->SetMapEditorCallback([this]()
	{
		MapEditorUI* editor = GUI->GetMapEditorUI();
		if (!editor) return;

		auto tile = std::make_shared<ATerrainTileActor>();
		tile->m_szName = L"Terrain";

		tile->m_iNumCols = editor->GetNumCols();
		tile->m_iNumRows = editor->GetNumRows();
		tile->m_fCellSize = editor->GetCellSize();

		tile->CreateTerrain(editor->GetTexturePath(), editor->GetShaderPath());
		tile->SetPosition(editor->GetPosition());
		tile->SetRotation(editor->GetRotation());
		tile->SetScale(editor->GetScale());

		OBJECT->AddActor(tile);
	});

	GUI->SetObjectEditorCallback([this](const char* texPath, const char* shaderPath, const char* objPath, Vec3 pos, Vec3 rot, Vec3 scale)
		{
			AssimpLoader loader;
			vector<MeshData> meshList = loader.Load(objPath);
			if (meshList.empty())
				return;

			auto meshComp = make_shared<UStaticMeshComponent>();
			meshComp->SetMeshPath(to_mw(objPath));

			auto meshRes = make_shared<UStaticMeshResources>();
			meshRes->SetVertexList(meshList[0].m_vVertexList);
			meshRes->SetIndexList(meshList[0].m_vIndexList);
			meshRes->Create();

			meshComp->SetMesh(meshRes);

			auto mat = make_shared<UMaterial>();
			mat->Load(
				std::wstring(texPath, texPath + strlen(texPath)),
				std::wstring(shaderPath, shaderPath + strlen(shaderPath))
			);
			meshComp->SetMaterial(mat);

			// Snap 적용 여부 확인
			if (GUI->GetObjectEditorUI()->IsSnapEnabled())
			{
				pos = GUI->GetObjectEditorUI()->SnapToGrid(pos, 10.0f);
			}

			auto actor = make_shared<APawn>();
			actor->m_szName = L"Object";

			actor->SetMeshComponent(meshComp);
			actor->SetPosition(pos);
			actor->SetRotation(rot);
			actor->SetScale(scale);

			OBJECT->AddActor(actor);
		});

	OBJECT->AddActor(m_pActor);
	OBJECT->AddActor(m_pCameraActor);
	OBJECT->AddActor(m_pSky);	
}

void TestSY::Update()
{
	CAMERA->Set3DCameraActor(m_pCameraActor);
	if (INPUT->GetButtonDown(O))
		CAMERA->Set3DCameraActor(m_pActor);

	if (INPUT->GetButton(I))
	{
		m_pActor->m_bDelete = true;
	}

	// Mouse Picking
	if (INPUT->GetButton(LCLICK))
		SetClickPos();
}

void TestSY::Render()
{	

}

void TestSY::Destroy()
{
}

void TestSY::SetClickPos()
{
	MouseRay m_vRay;

	m_vRay.Click();

	//// Ray 가시화
	//auto pActor = make_shared<APawn>();
	//
	//auto pMesh = UStaticMeshComponent::CreateRay(m_vRay.position, m_vRay.EndPos);
	//pMesh->GetMesh()->SetTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
	//
	//pActor->SetMeshComponent(pMesh);
	//
	//auto pMaterial = make_shared<UMaterial>();
	//pMaterial->Load(L"", L"../Resources/Shader/DefaultColor.hlsl");	
	//pMesh->SetMaterial(pMaterial);
	//
	//OBJECT->AddActor(pActor);

	//// Check GetInterSection, PointInPolygon
	//Vec3 v0 = Vec3(-0.5f, -0.5f, 0.f);
	//Vec3 v1 = Vec3(-0.5f, +0.5f, 0.f);
	//Vec3 v2 = Vec3(+0.5f, -0.5f, 0.f);
	//Vec3 normal = Vec3(0.f, 0.f, -1.f);
	//Vec3 inter;
	//
	//bool col = Collision::CheckMousePicking(m_vRay, v0, v1, v2, normal, inter);
	//
	//if (col)
	//	int i = 0;

	Vec3 inter;

	auto vList = OBJECT->GetActorList();

	for (auto& iter : vList)
	{
		auto pActor = iter.second;
		if (pActor->m_bCollision == false)continue;
		auto shapeComponent = pActor->GetShapeComponent();
		if (shapeComponent == nullptr) continue;

		if (shapeComponent->GetShapeType() == ShapeType::ST_BOX)
		{
			auto boxComponent = static_cast<UBoxComponent*>(shapeComponent.get());

			if (Collision::CheckAABBToRay(m_vRay, boxComponent->GetBounds(), inter))
			{
				int i = 0;
			}
		}
		else if (shapeComponent->GetShapeType() == ShapeType::ST_SPHERE)
		{
			int i = 0;
		}
		else if (shapeComponent->GetShapeType() == ShapeType::ST_CAPSULE)
		{
			int i = 0;
		}
		else
		{
			int i = 0;
		}
	}
}

void TestSY::LoadAllPrefabs(const std::string& extension)
{
	auto files = PREFAB->GetPrefabFileList("../Resources/Prefab/", extension);

	for (const auto& file : files)
	{
		if (extension == ".map.json")
		{
			PrefabMapData mapData;
			if (PREFAB->LoadMapTile(file, mapData))
			{
				auto tile = std::make_shared<ATerrainTileActor>();
				tile->m_iNumCols = mapData.Cols;
				tile->m_iNumRows = mapData.Rows;
				tile->m_fCellSize = mapData.CellSize;
				tile->CreateTerrain(to_mw(mapData.TexturePath), to_mw(mapData.ShaderPath));
				tile->SetPosition(mapData.Position);
				tile->SetRotation(mapData.Rotation);
				tile->SetScale(mapData.Scale);
				OBJECT->AddActor(tile);
			}
		}
		else if (extension == ".character.json")
		{
			PrefabCharacterData characterData;
			if (PREFAB->LoadCharacter(file, characterData))
			{
				auto actor = std::make_shared<AActor>(); // 필요에 따라 캐릭터 타입으로 변경
				actor->SetPosition(characterData.Translation);
				actor->SetRotation(characterData.Rotation);
				actor->SetScale(characterData.Scale);
				OBJECT->AddActor(actor);
			}
		}
		else if (extension == ".object.json")
		{
			PrefabData objData;
			if (PREFAB->Load(file, objData))
			{
				auto actor = std::make_shared<AActor>(); // 필요에 따라 오브젝트 타입으로 변경
				actor->SetPosition(objData.Translation);
				actor->SetRotation(objData.Rotation);
				actor->SetScale(objData.Scale);
				OBJECT->AddActor(actor);
			}
		}
	}
}
