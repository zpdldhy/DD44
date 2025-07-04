#include "pch.h"
#include "TestYoooooon.h"

// Manager
#include "Input.h"
#include "ImGuiCore.h"
#include "ObjectManager.h"
#include "CameraManager.h"
#include "LightManager.h"
#include "CollisionManager.h"
#include "UIManager.h"
#include "MeshLoader.h"
#include "AssimpLoader.h"
#include "DxWrite.h"
#include "PrefabToActor.h"
#include "ScriptManager.h"
#include "EffectManager.h"

// Actor
#include "ATerrainTileActor.h"
#include "ACameraActor.h"
#include "ALight.h"
#include "ASky.h"
#include "AUIActor.h"

// Component
#include "UStaticMeshComponent.h"
#include "UBoxComponent.h"
#include "USphereComponent.h"

// Script
#include "EngineCameraMoveScript.h"
#include "PlayerMoveScript.h"

void TestYoooooon::Init()
{
	SetupEditorCallbacks();

	SCRIPT->Init();
	PToA->Init();
	EFFECT->Init();

	m_vGround = PToA->LoadAllPrefabs(".map.json");
	OBJECT->AddActorList(m_vGround);

	SetupEngineCamera();
	//SetupSkybox();

	CreateCollisionObject();
}

void TestYoooooon::Tick()
{
	if (INPUT->GetButton(O))
	{
		if (m_bEnginCamera)
		{
			m_bEnginCamera = false;
			CAMERA->Set3DCameraActor(m_pCinemaCamera);
		}
		else
		{
			m_bEnginCamera = true;
			CAMERA->Set3DCameraActor(m_pCameraActor);
		}
	}

	//if (INPUT->GetButton(LCLICK))
		//ClickMouse();

	//CheckCollision();
}

void TestYoooooon::Render()
{

}

void TestYoooooon::Destroy()
{
}

void TestYoooooon::SetupEngineCamera()
{
	m_pCameraActor = make_shared<ACameraActor>();

	m_pCameraActor->AddScript(make_shared<EngineCameraMoveScript>());
	m_pCameraActor->m_szName = L"EnginCamera";
	m_pCameraActor->SetPosition(Vec3(0.f, 10.f, -50.f));
	m_pCameraActor->SetRotation(Vec3(0.f, -0.3f, 0.f));

	CAMERA->Set3DCameraActor(m_pCameraActor);
	OBJECT->AddActor(m_pCameraActor);

	m_pCinemaCamera = make_shared<ACameraActor>();
	m_pCinemaCamera->m_szName = L"EnginCamera";
	m_pCinemaCamera->SetPosition(Vec3(0.f, 20.f, -30.f));

	OBJECT->AddActor(m_pCinemaCamera);
}

void TestYoooooon::SetupSkybox()
{
	auto pSky = make_shared<ASky>();
	pSky->m_szName = L"Sky";
	auto pSkyMesh = UStaticMeshComponent::CreateSphere(20, 20);
	pSky->SetMeshComponent(pSkyMesh);

	shared_ptr<UMaterial> material = make_shared<UMaterial>();
	material->Load(L"../Resources/Texture/Sky.jpg", L"../Resources/Shader/Sky.hlsl");
	pSkyMesh->SetMaterial(material);

	OBJECT->AddActor(pSky);
}

void TestYoooooon::SetupSunLight()
{
	auto pSunLight = make_shared<ALight>();
	pSunLight->m_szName = L"SunLight";
	pSunLight->GetLightComponent()->SetDirection({ 0, -1.f, 0 });
	pSunLight->GetLightComponent()->SetAmbientColor(Vec3(1.0f, 1.0f, 1.0f));
	pSunLight->GetLightComponent()->SetAmbientPower(0.3f);
	pSunLight->SetPosition(Vec3(0, 100.0f, 0));
	pSunLight->SetScale(Vec3(10.0f, 10.0f, 10.0f));
	OBJECT->AddActor(pSunLight);

	LIGHT->Clear();
	LIGHT->RegisterLight(pSunLight);
}

void TestYoooooon::SetupEditorCallbacks()
{
	SetupCharacterEditorCallback();
	SetupMapEditorCallback();
	SetupObjectEditorCallback();
	SetupUIEditorCallback();
}

void TestYoooooon::SetupCharacterEditorCallback()
{
	GUI->SetCharacterEditorCallback(
		[](std::shared_ptr<UMeshComponent> rootComponent, const Vec3& position,
			const Vec3& rotation,
			const Vec3& scale,
			CameraComponentData camera,
			ShapeComponentData shape,
			int scriptType)
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

			if (camera.isUse)
			{
				auto cam = std::make_shared<UCameraComponent>();
				cam->SetLocalPosition(Vec3(camera.Position));
				cam->SetLocalRotation(Vec3(camera.Rotation));
				cam->SetPerspective(camera.Fov, camera.Aspect, camera.Near, camera.Far);
				actor->SetCameraComponent(cam);
			}

			if (shape.isUse)
			{
				shared_ptr<UShapeComponent> shapeComp = nullptr;

				if (shape.eShapeType == ShapeType::ST_BOX)
					shapeComp = std::make_shared<UBoxComponent>();
				//else if (shape.eShapeType == ShapeType::ST_SPHERE)

				shapeComp->SetLocalScale(Vec3(shape.Scale));
				shapeComp->SetLocalPosition(Vec3(shape.Position));
				shapeComp->SetLocalRotation(Vec3(shape.Rotation));
				shapeComp->SetCollisionEnabled(CollisionEnabled::CE_QUERYONLY);

				actor->SetShapeComponent(shapeComp);
			}

			if (scriptType == 1) actor->AddScript(std::make_shared<PlayerMoveScript>());
			//if (scriptType == 2) actor->AddScript(std::make_shared<EnemyAIScript>());

			OBJECT->AddActor(actor);
		});
}

void TestYoooooon::SetupMapEditorCallback()
{
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

			auto mesh = tile->m_pTerrainMeshComponent->GetMesh();
			auto newVertexList = mesh->GetVertexList();

			int vertexCountX = tile->m_iNumCols + 1;
			int row = editor->GetSelectedRow();
			int col = editor->GetSelectedCol();
			float centerHeight = editor->GetTargetHeight();

			int topLeft = row * vertexCountX + col;
			int topRight = topLeft + 1;
			int bottomLeft = (row + 1) * vertexCountX + col;
			int bottomRight = bottomLeft + 1;

			if (bottomRight < newVertexList.size())
			{
				newVertexList[topLeft].pos.y = centerHeight;
				newVertexList[topRight].pos.y = centerHeight;
				newVertexList[bottomLeft].pos.y = centerHeight;
				newVertexList[bottomRight].pos.y = centerHeight;
			}

			mesh->SetVertexList(newVertexList);
			mesh->Create();

			OBJECT->AddActor(tile);
		});
}

void TestYoooooon::SetupObjectEditorCallback()
{
	GUI->SetObjectEditorCallback([this](const char* texPath, const char* shaderPath, const char* objPath, Vec3 pos, Vec3 rot, Vec3 scale, Vec3 SpecularColor, float shininess, Vec3 emissiveColor, float emissivepower, ShapeComponentData shapeData)
		{
			ActorLoader al;
			al.LoadOne(objPath);
			auto meshComp = make_shared<UStaticMeshComponent>();
			meshComp->SetMeshPath(to_mw(objPath));
			auto resources = al.LoadMeshResources();
			meshComp->SetMesh(dynamic_pointer_cast<UStaticMeshResources>(resources[0]));

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

			if (shapeData.isUse)
			{
				shared_ptr<UShapeComponent> shapeComp = nullptr;

				if (shapeData.eShapeType == ShapeType::ST_BOX)
					shapeComp = std::make_shared<UBoxComponent>();
				//else if (shapeData.eShapeType == ShapeType::ST_SPHERE)

				shapeComp->SetLocalScale(Vec3(shapeData.Scale));
				shapeComp->SetLocalPosition(Vec3(shapeData.Position));
				shapeComp->SetLocalRotation(Vec3(shapeData.Rotation));
				shapeComp->SetCollisionEnabled(CollisionEnabled::CE_QUERYONLY);

				actor->SetShapeComponent(shapeComp);
			}

			OBJECT->AddActor(actor);
		});
}

void TestYoooooon::SetupUIEditorCallback()
{
	GUI->SetUIEditorCallback([this](shared_ptr<AUIActor> uiActor, PrefabUIData _prefabData)
		{
			uiActor->m_szName = L"UI";
			auto meshComp = UStaticMeshComponent::CreatePlane();
			uiActor->SetMeshComponent(meshComp);
			uiActor->m_bTextUI = _prefabData.isTextUI;

			auto mat = make_shared<UMaterial>();
			mat->Load(
				to_mw(_prefabData.IdleTexturePath),
				to_mw(_prefabData.ShaderPath)
			);
			meshComp->SetMaterial(mat);

			uiActor->SetIdleTexture(TEXTURE->Get(to_mw(_prefabData.IdleTexturePath)));
			uiActor->SetHoverTexture(TEXTURE->Get(to_mw(_prefabData.HoverTexturePath)));
			uiActor->SetActiveTexture(TEXTURE->Get(to_mw(_prefabData.ActiveTexturePath)));
			uiActor->SetSelectTexture(TEXTURE->Get(to_mw(_prefabData.SelectedTexturePath)));

			uiActor->SetMeshComponent(meshComp);
			uiActor->SetPosition(Vec3(_prefabData.transform.Position));
			uiActor->SetRotation(Vec3(_prefabData.transform.Rotation));
			uiActor->SetScale(Vec3(_prefabData.transform.Scale));
			uiActor->SetColor(Vec4(_prefabData.color));

			uiActor->SetFontSize(_prefabData.FontSize);
			uiActor->SetFontPath(to_mw(_prefabData.FontPath));
			uiActor->SetText(to_mw(_prefabData.Text));

			uiActor->SetSliceData(Vec4(_prefabData.SliceUV));

			uiActor->SetPrefabData(_prefabData);

			UI->AddUI(uiActor);
		});
}

void TestYoooooon::CreateCollisionObject()
{
	{
		m_pBox = make_shared<AActor>();

		auto pCube = UStaticMeshComponent::CreateCube();
		m_pBox->SetMeshComponent(pCube);
		m_pBox->m_szName = L"Stair";

		auto pMaterial = make_shared<UMaterial>();
		pMaterial->Load(L"../Resources/Texture/kkongchi.jpg", L"../Resources/Shader/Default.hlsl");
		pCube->SetMaterial(pMaterial);

		auto boxCol = make_shared<UBoxComponent>();
		boxCol->SetCollisionEnabled(CollisionEnabled::CE_QUERYANDPHYSICS);
		m_pBox->SetShapeComponent(boxCol);

		m_pBox->SetScale(Vec3(30.f, 30.f, 30.f));
		m_pBox->SetRotation(Vec3(0.f, 0.f, DD_PI / 8.f));
		m_pBox->SetPosition(Vec3(100.f, -10.f, 200.f));

		OBJECT->AddActor(m_pBox);
	}

	{
		m_pSphere = make_shared<AActor>();

		auto pCube = UStaticMeshComponent::CreateSphere(20.f, 20.f);
		m_pSphere->SetMeshComponent(pCube);
		m_pSphere->m_szName = L"Stair";

		auto pMaterial = make_shared<UMaterial>();
		pMaterial->Load(L"../Resources/Texture/kkongchi.jpg", L"../Resources/Shader/Default.hlsl");
		pCube->SetMaterial(pMaterial);

		auto boxCol = make_shared<USphereComponent>();
		boxCol->SetCollisionEnabled(CollisionEnabled::CE_QUERYANDPHYSICS);
		m_pSphere->SetShapeComponent(boxCol);

		m_pSphere->SetScale(Vec3(30.f, 30.f, 30.f));
		m_pSphere->SetRotation(Vec3(0.f, 0.f, 0.f));
		m_pSphere->SetPosition(Vec3(-50.f, -10.f, -50.f));

		OBJECT->AddActor(m_pSphere);
	}
}

void TestYoooooon::ClickMouse()
{
	//m_Cursor.Click();

	//auto boxCom = static_pointer_cast<UBoxComponent>(m_pPlayer->GetShapeComponent());

	//Vec3 vinter;

	//if (Collision::CheckRayToOBB(m_Cursor, boxCom->GetBounds(), vinter))
	//{
	//	int a = 0;
	//}
}

void TestYoooooon::CheckCollision()
{
	//COLLITION->CheckCollision(m_pPlayer, m_pBox);
	//COLLITION->CheckCollision(m_pPlayer, m_pSphere);

	//for (auto& ground : m_vGround)
	//	COLLITION->CheckCollision(m_pPlayer, ground);
}