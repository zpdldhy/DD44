#include "pch.h"
#include "TestYoooooon.h"
#include "ImGuiCore.h"
#include "ObjectManager.h"
#include "CameraManager.h"
#include "ACameraActor.h"
#include "ASky.h"
#include "ATerrainTileActor.h"
#include "UStaticMeshComponent.h"
#include "EngineCameraMoveScript.h"
#include "PlayerMoveScript.h"
#include "AssimpLoader.h"
#include "LightManager.h"
#include "ALight.h"
#include "MeshLoader.h"
#include "Input.h"
#include "UBoxComponent.h"
#include "AUIActor.h"
#include "UIManager.h"
#include "ShapeData.h"
#include "CollisionManager.h"

void TestYoooooon::Init()
{
	actorLoader.LoadAllAsset();
	meshLoader.SetMesh(actorLoader.LoadMeshMap());
	meshLoader.SetAnim(actorLoader.LoadAnimMap());

	SetupEditorCallbacks();

	LoadAllPrefabs(".map.json");
	//LoadAllPrefabs(".object.json");
	LoadAllPrefabs(".objects.json");
	LoadAllPrefabs(".character.json");
	LoadAllPrefabs(".ui.json");

	SetupEngineCamera();
	SetupSkybox();
	SetupSunLight();
}

void TestYoooooon::Update()
{
	if (INPUT->GetButton(O))
	{
		if (m_bEnginCamera)
		{
			m_bEnginCamera = false;
			CAMERA->Set3DCameraActor(m_pPlayer);
		}
		else
		{
			m_bEnginCamera = true;
			CAMERA->Set3DCameraActor(m_pCameraActor);
		}
	}

	if (INPUT->GetButton(LCLICK))
		ClickMouse();
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

	m_pCameraActor->SetPosition({ 0.0f, 10.0f, 0.0f });
	m_pCameraActor->AddScript(make_shared<EngineCameraMoveScript>());
	m_pCameraActor->m_szName = L"EnginCamera";

	CAMERA->Set3DCameraActor(m_pCameraActor);
	OBJECT->AddActor(m_pCameraActor);
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
	LIGHTMANAGER->Init();

	auto pSunLight = make_shared<ALight>();
	pSunLight->m_szName = L"SunLight";
	pSunLight->GetLightComponent()->SetDirection({ 0, -1.f, 0 });
	pSunLight->GetLightComponent()->SetAmbientColor(Vec3(1.0f, 1.0f, 1.0f));
	pSunLight->GetLightComponent()->SetAmbientPower(0.3f);
	pSunLight->SetPosition(Vec3(0, 100.0f, 0));
	pSunLight->SetScale(Vec3(10.0f, 10.0f, 10.0f));
	OBJECT->AddActor(pSunLight);

	LIGHTMANAGER->Clear();
	LIGHTMANAGER->RegisterLight(pSunLight);
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
	GUI->SetUIEditorCallback([this](shared_ptr<AUIActor> uiActor, const char* texPath, const char* shaderPath, TransformData actorData, Vec4 sliceUV)
		{
			uiActor->m_szName = L"UI";
			auto meshComp = UStaticMeshComponent::CreatePlane();
			uiActor->SetMeshComponent(meshComp);

			auto mat = make_shared<UMaterial>();
			mat->SetUseEffect(false);
			mat->Load(
				std::wstring(texPath, texPath + strlen(texPath)),
				std::wstring(shaderPath, shaderPath + strlen(shaderPath))
			);
			meshComp->SetMaterial(mat);

			uiActor->SetIdleTexture(TEXTURE->Get(to_mw(texPath)));
			uiActor->SetHoverTexture(TEXTURE->Get(to_mw(texPath)));
			uiActor->SetActiveTexture(TEXTURE->Get(to_mw(texPath)));
			uiActor->SetSelectTexture(TEXTURE->Get(to_mw(texPath)));

			uiActor->SetMeshComponent(meshComp);
			uiActor->SetPosition(Vec3(actorData.Position));
			uiActor->SetRotation(Vec3(actorData.Rotation));
			uiActor->SetScale(Vec3(actorData.Scale));
			uiActor->SetSliceData(sliceUV);

			UI->AddUI(uiActor);
		});
}

void TestYoooooon::LoadAllPrefabs(const std::string& extension)
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
				tile->m_szName = L"Terrain";
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

				ActorLoader actorLoader;
				actorLoader.LoadAllAsset();
				MeshLoader meshLoader;
				meshLoader.SetMesh(actorLoader.LoadMeshMap());
				meshLoader.SetAnim(actorLoader.LoadAnimMap());

				shared_ptr<UMeshComponent> meshComponent = meshLoader.Make(characterData.MeshPath.c_str());

				actor->SetMeshComponent(meshComponent);

				actor->m_szName = L"Character";
				actor->SetPosition(Vec3(characterData.transform.Position));
				actor->SetRotation(Vec3(characterData.transform.Rotation));
				actor->SetScale(Vec3(characterData.transform.Scale));

				if (characterData.ScriptType == 1) actor->AddScript(std::make_shared<PlayerMoveScript>());

				m_pPlayer = actor;

				if (characterData.camera.isUse)
				{
					auto cameraComponent = make_shared<UCameraComponent>();
					cameraComponent->SetLocalPosition(Vec3(characterData.camera.Position));
					cameraComponent->SetLocalRotation(Vec3(characterData.camera.Rotation));
					cameraComponent->SetPerspective(characterData.camera.Fov, characterData.camera.Aspect, characterData.camera.Near, characterData.camera.Far);
					actor->SetCameraComponent(cameraComponent);
				}

				if (characterData.shape.isUse)
				{
					shared_ptr<UShapeComponent> shapeComponent = nullptr;
					if (static_cast<ShapeType>(characterData.shape.eShapeType) == ShapeType::ST_BOX)
						shapeComponent = make_shared<UBoxComponent>();
					//else if (static_cast<ShapeType>(characterData.shape.eShapeType) == ShapeType::ST_SPHERE)

					shapeComponent->SetLocalScale(Vec3(characterData.shape.Scale));
					shapeComponent->SetLocalPosition(Vec3(characterData.shape.Position));
					shapeComponent->SetLocalRotation(Vec3(characterData.shape.Rotation));
					shapeComponent->SetCollisionEnabled(CollisionEnabled::CE_QUERYONLY);
					actor->SetShapeComponent(shapeComponent);
				}


				OBJECT->AddActor(actor);
			}
		}
		else if (extension == ".object.json")
		{
			PrefabObjectData objData;
			if (PREFAB->LoadObject(file, objData))
			{
				auto meshComp = make_shared<UStaticMeshComponent>();
				meshComp->SetMeshPath(to_mw(objData.MeshPath));

				auto meshRes = make_shared<UStaticMeshResources>();
				AssimpLoader loader;
				vector<MeshData> meshList = loader.Load(objData.MeshPath.c_str());
				if (!meshList.empty())
				{
					meshRes->SetVertexList(meshList[0].m_vVertexList);
					meshRes->SetIndexList(meshList[0].m_vIndexList);
					meshRes->Create();
					meshComp->SetMesh(meshRes);
				}

				auto material = make_shared<UMaterial>();
				material->Load(to_mw(objData.TexturePath), to_mw(objData.ShaderPath));
				meshComp->SetMaterial(material);

				auto obj = make_shared<APawn>();
				obj->m_szName = L"Object";
				obj->SetMeshComponent(meshComp);
				obj->SetPosition(objData.Position);
				obj->SetRotation(objData.Rotation);
				obj->SetScale(objData.Scale);

				OBJECT->AddActor(obj);
			}
		}
		else if (extension == ".objects.json")
		{
			std::vector<PrefabObjectData> objList;
			if (PREFAB->LoadObjectArray(file, objList))
			{
				for (auto& objData : objList)
				{
					auto meshComp = make_shared<UStaticMeshComponent>();
					meshComp->SetMeshPath(to_mw(objData.MeshPath));

					auto resources = actorLoader.LoadOneRes(objData.MeshPath);
					meshComp->SetMesh(dynamic_pointer_cast<UStaticMeshResources>(resources));

					auto material = make_shared<UMaterial>();
					material->Load(to_mw(objData.TexturePath), to_mw(objData.ShaderPath));
					meshComp->SetMaterial(material);

					auto obj = make_shared<APawn>();
					obj->m_szName = L"Object";
					obj->SetMeshComponent(meshComp);
					obj->SetPosition(objData.Position);
					obj->SetRotation(objData.Rotation);
					obj->SetScale(objData.Scale);

					OBJECT->AddActor(obj);
				}
			}
		}
		else if (extension == ".ui.json")
		{
			PrefabUIData uiData;
			if (PREFAB->LoadUI(file, uiData))
			{
				auto uiActor = make_shared<AUIActor>();
				uiActor->m_szName = to_mw(uiData.Name);

				auto meshComp = UStaticMeshComponent::CreatePlane();
				uiActor->SetMeshComponent(meshComp);

				auto mat = make_shared<UMaterial>();
				mat->SetUseEffect(false);
				mat->Load(L"", to_mw(uiData.ShaderPath));
				meshComp->SetMaterial(mat);

				uiActor->SetIdleTexture(TEXTURE->Get(to_mw(uiData.IdleTexturePath)));
				uiActor->SetHoverTexture(TEXTURE->Get(to_mw(uiData.HoverTexturePath)));
				uiActor->SetActiveTexture(TEXTURE->Get(to_mw(uiData.ActiveTexturePath)));
				uiActor->SetSelectTexture(TEXTURE->Get(to_mw(uiData.SelectedTexturePath)));

				uiActor->SetMeshComponent(meshComp);
				uiActor->SetPosition(Vec3(uiData.transform.Position));
				uiActor->SetRotation(Vec3(uiData.transform.Rotation));
				uiActor->SetScale(Vec3(uiData.transform.Scale));
				uiActor->SetSliceData(Vec4(uiData.SliceUV));

				uiActor->SetPrefabData(uiData);

				UI->AddUI(uiActor);
			}
		}

	}
}

void TestYoooooon::ClickMouse()
{
	m_Cursor.Click();

	shared_ptr<AActor> pActor = nullptr;

	if (Collision::CheckRayCollision(m_Cursor, OBJECT->GetActorIndexList(), pActor))
	{
		if (pActor != nullptr)
			int a = 0;
	}
}
