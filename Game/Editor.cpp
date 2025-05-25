#include "pch.h"
#include "Editor.h"
#include "ImGuiCore.h"
#include "ObjectManager.h"
#include "CameraManager.h"
#include "CollisionManager.h"
#include "ACameraActor.h"
#include "ASky.h"
#include "ATerrainTileActor.h"
#include "UStaticMeshComponent.h"
#include "EngineCameraMoveScript.h"
#include "PlayerMoveScript.h"
#include "AssimpLoader.h"
#include "LightManager.h"
#include "ALight.h"
#include "UMeshComponent.h"
//#include "MeshLoader.h"
#include "Input.h"
#include "UBoxComponent.h"
#include "AUIActor.h"
#include "UIManager.h"
#include "AParticleActor.h"
#include "ParticleManager.h"
#include "Timer.h"

bool bRunGame = true;

//#undef RUN_GAME
void Editor::Init()
{
	// Asset 로딩
	actorLoader.LoadAllAsset();
	meshLoader.SetMesh(actorLoader.LoadMeshMap());
	meshLoader.SetAnim(actorLoader.LoadAnimMap());
	
	{
		m_pSlashMaterial = make_shared<UMaterial>();
		m_pSlashMaterial->Load(
			L"../Resources/Texture/sword_slash_texture1.png",
			L"../Resources/Shader/Slash.hlsl"
		);
	}
	
	SetupEditorCallbacks();

	LoadAllPrefabs(".map.json");
	LoadAllPrefabs(".objects.json");
	LoadAllPrefabs(".object.json");
	LoadAllPrefabs(".character.json");
	LoadAllPrefabs(".ui.json");

	SetupEngineCamera();
	SetupSkybox();
	SetupSunLight();
}

void Editor::Update()
{
	Slash();
	
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

	if (INPUT->GetButtonDown(LCLICK))
	{
		SetClickPos();

		UINT selectedID = GUI->GetActorListUI()->GetSelectedActorID();
		if (selectedID > 0 && OBJECT->GetActorList().count(selectedID) > 0)
		{
			if (!m_pGizmoCore)
			{
				SetupGizmo();
			}

			auto& actorMap = OBJECT->GetActorList();
			auto _actor = actorMap.find(GUI->GetActorListUI()->GetSelectedActorID());
			if (_actor != actorMap.end())
			{
				auto actor = _actor->second;
				SetGizmoPosition(actor->GetPosition());
			}

			auto _axis = actorMap.find(GUI->GetActorListUI()->GetSelectedGizmoAxis());
			if (_axis != actorMap.end())
			{
				auto actor = _actor->second;
				auto axis = _axis->second;
				if (!actor || !axis) return;

				// 드래그 처리
				POINT curMouse = INPUT->GetMousePos();
				float deltaX = static_cast<float>(curMouse.x - m_vPrevMouse.x);
				float deltaY = static_cast<float>(curMouse.y - m_vPrevMouse.y);
				Vec3 newPos = m_vDragStartPos;

				if (axis == m_pGizmoX)
					newPos.x += deltaX * 0.07f;
				else if (axis == m_pGizmoY)
					newPos.y -= deltaY * 0.07f;
				else if (axis == m_pGizmoZ)
					newPos.z += deltaX * 0.07f;

				actor->SetPosition(newPos);
				SetGizmoPosition(actor->GetPosition());
			}
		}
	}
	else if (INPUT->GetButtonUp(LCLICK))
	{
		m_bDragging = false;
		GUI->GetActorListUI()->SetSelectedGizmoAxis(-1);
	}
}

void Editor::Render()
{

}

void Editor::Destroy()
{
}

void Editor::SetupEngineCamera()
{
	m_pCameraActor = make_shared<ACameraActor>();

	m_pCameraActor->SetPosition({ 0.0f, 10.0f, 0.0f });
	m_pCameraActor->AddScript(make_shared<EngineCameraMoveScript>());
	m_pCameraActor->m_szName = L"EnginCamera";

	CAMERA->Set3DCameraActor(m_pCameraActor);
	OBJECT->AddActor(m_pCameraActor);
}

void Editor::SetupSkybox()
{
	m_pSky = make_shared<ASky>();
	m_pSky->m_szName = L"Sky";
	m_pSkyMesh = UStaticMeshComponent::CreateSphere(20, 20);
	m_pSky->SetMeshComponent(m_pSkyMesh);

	shared_ptr<UMaterial> material = make_shared<UMaterial>();
	material->Load(L"../Resources/Texture/Sky.jpg", L"../Resources/Shader/Sky.hlsl");
	m_pSkyMesh->SetMaterial(material);

	OBJECT->AddActor(m_pSky);
}

void Editor::SetupSunLight()
{
	LIGHTMANAGER->Init();

	m_pSunLight = make_shared<ALight>();
	m_pSunLight->GetLightComponent()->SetDirection({ 0, -1.f, 0 });
	m_pSunLight->GetLightComponent()->SetAmbientColor(Vec3(1.0f, 1.0f, 1.0f));
	m_pSunLight->GetLightComponent()->SetAmbientPower(0.3f);
	m_pSunLight->SetPosition(Vec3(0, 100.0f, 0));
	m_pSunLight->SetScale(Vec3(10.0f, 10.0f, 10.0f));
	OBJECT->AddActor(m_pSunLight);

	LIGHTMANAGER->Clear();
	LIGHTMANAGER->RegisterLight(m_pSunLight);
}

void Editor::SetupGizmo()
{
	m_pGizmoCore = make_shared<AActor>();
	m_pGizmoCore->m_szName = L"Gizmo";
	shared_ptr<UMeshComponent> meshComponent =
		meshLoader.Make("../Resources/Asset/gizmo.mesh.json");
	m_pGizmoCore->SetMeshComponent(meshComponent);
	m_pGizmoCore->SetScale(Vec3(0.04f, 0.04f, 0.04f));
	OBJECT->AddActor(m_pGizmoCore);

	float _boxScale = 5.0f;
	float _boxPosition = 4.0f;

	m_pGizmoX = make_shared<AActor>();
	m_pGizmoX->m_szName = L"Gizmo";
	m_pGizmoX->SetMeshComponent(meshComponent->GetChild(0));
	shared_ptr<UBoxComponent> _pXBox = std::make_shared<UBoxComponent>();
	_pXBox->m_bVisible = false;
	_pXBox->SetShapeColor(Vec4(1.0f, 0.f, 0.f, 0.f));
	_pXBox->SetLocalScale(Vec3(_boxScale, 0.4f, 0.4f));
	_pXBox->SetLocalPosition(Vec3(_boxPosition, 0.0f, 0.0f));
	_pXBox->SetCollisionEnabled(CollisionEnabled::CE_QUERYONLY);
	m_pGizmoX->SetShapeComponent(_pXBox);
	m_pGizmoX->SetScale(m_pGizmoCore->GetScale());
	m_pGizmoX->SetPosition(m_pGizmoCore->GetPosition());
	OBJECT->AddActor(m_pGizmoX);

	m_pGizmoY = make_shared<AActor>();
	m_pGizmoY->m_szName = L"Gizmo";
	m_pGizmoY->SetMeshComponent(meshComponent->GetChild(1));
	shared_ptr<UBoxComponent> _pYBox = std::make_shared<UBoxComponent>();
	_pYBox->m_bVisible = false;
	_pYBox->SetShapeColor(Vec4(0.f, 1.0f, 0.f, 0.f));
	_pYBox->SetLocalScale(Vec3(0.4f, _boxScale, 0.4f));
	_pYBox->SetLocalPosition(Vec3(0.0f, _boxPosition, 0.0f));
	_pYBox->SetCollisionEnabled(CollisionEnabled::CE_QUERYONLY);
	m_pGizmoY->SetShapeComponent(_pYBox);
	m_pGizmoY->SetScale(m_pGizmoCore->GetScale());
	m_pGizmoY->SetPosition(m_pGizmoCore->GetPosition());
	OBJECT->AddActor(m_pGizmoY);

	m_pGizmoZ = make_shared<AActor>();
	m_pGizmoZ->m_szName = L"Gizmo";
	m_pGizmoZ->SetMeshComponent(meshComponent->GetChild(2));
	shared_ptr<UBoxComponent> _pZBox = std::make_shared<UBoxComponent>();
	_pZBox->m_bVisible = false;
	_pZBox->SetShapeColor(Vec4(0.f, 0.f, 1.0f, 0.f));
	_pZBox->SetLocalScale(Vec3(0.4f, 0.4f, _boxScale));
	_pZBox->SetLocalPosition(Vec3(0.0f, 0.0f, _boxPosition));
	_pZBox->SetCollisionEnabled(CollisionEnabled::CE_QUERYONLY);
	m_pGizmoZ->SetShapeComponent(_pZBox);
	m_pGizmoZ->SetScale(m_pGizmoCore->GetScale());
	m_pGizmoZ->SetPosition(m_pGizmoCore->GetPosition());
	OBJECT->AddActor(m_pGizmoZ);

	//shared_ptr<UShapeComponent> _pRootShape = std::make_shared<UShapeComponent>();
	//_pRootShape->SetCollisionEnabled(CollisionEnabled::CE_QUERYONLY);
	//_pRootShape->SetLocalScale(Vec3(1.0f, 1.0f, 1.0f));

	//shared_ptr<UBoxComponent> _pXBox = nullptr;
	//_pXBox = std::make_shared<UBoxComponent>();
	//_pXBox->SetLocalScale(Vec3(1.0f, 0.1f, 0.1f));
	//_pXBox->SetLocalPosition(Vec3(1.2f, 0.0f, 0.0f));
	//_pXBox->SetCollisionEnabled(CollisionEnabled::CE_QUERYONLY);
	//_pRootShape->AddChild(_pXBox);

	//shared_ptr<UBoxComponent> _YBox = nullptr;
	//_YBox = std::make_shared<UBoxComponent>();
	//_YBox->SetLocalScale(Vec3(0.1f, 1.0f, 0.1f));
	//_YBox->SetLocalPosition(Vec3(0.0f, 1.2f, 0.0f));
	//_YBox->SetCollisionEnabled(CollisionEnabled::CE_QUERYONLY);
	//_pRootShape->AddChild(_YBox);

	//shared_ptr<UBoxComponent> _ZBox = nullptr;
	//_ZBox = std::make_shared<UBoxComponent>();
	//_ZBox->SetLocalScale(Vec3(0.1f, 0.1f, 1.0f));
	//_ZBox->SetLocalPosition(Vec3(0.0f, 0.0f, 1.2f));
	//_ZBox->SetCollisionEnabled(CollisionEnabled::CE_QUERYONLY);
	//_pRootShape->AddChild(_ZBox);

	//m_pGizmo->SetShapeComponent(_pRootShape);

	//m_pGizmo->SetScale(Vec3(0.01f, 0.01f, 0.01f));
	//m_pGizmo->SetPosition(Vec3(0, 10.0f, 0));
	//OBJECT->AddActor(m_pGizmo);
}

void Editor::SetGizmoPosition(Vec3 _pos)
{
	m_pGizmoCore->SetPosition(Vec3(_pos.x, _pos.y + 1.0f, _pos.z));
	m_pGizmoX->SetPosition(m_pGizmoCore->GetPosition());
	m_pGizmoY->SetPosition(m_pGizmoCore->GetPosition());
	m_pGizmoZ->SetPosition(m_pGizmoCore->GetPosition());
}

void Editor::SetupEditorCallbacks()
{
	SetupCharacterEditorCallback();
	SetupMapEditorCallback();
	SetupObjectEditorCallback();
	SetupUIEditorCallback();
	SetupParticleEditorCallback();
}

void Editor::SetupCharacterEditorCallback()
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

void Editor::SetupMapEditorCallback()
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

void Editor::SetupObjectEditorCallback()
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

void Editor::SetupUIEditorCallback()
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

void Editor::SetClickPos()
{
	MouseRay m_vRay;
	m_vRay.Click();

	shared_ptr<AActor> pActor = nullptr;
	if (Collision::CheckRayCollision(m_vRay, OBJECT->GetActorIndexList(), pActor))
	{
		if (!pActor) return;

		if (pActor->m_szName != L"Gizmo")
		{
			GUI->GetActorListUI()->SetSelectedActorID(pActor->m_Index);
			GUI->GetActorListUI()->SetSelectedGizmoAxis(-1);
		}
		else
		{
			if (pActor == m_pGizmoX)
				GUI->GetActorListUI()->SetSelectedGizmoAxis(pActor->m_Index);
			else if (pActor == m_pGizmoY)
				GUI->GetActorListUI()->SetSelectedGizmoAxis(pActor->m_Index);
			else if (pActor == m_pGizmoZ)
				GUI->GetActorListUI()->SetSelectedGizmoAxis(pActor->m_Index);

			m_pSelectedGizmo = pActor;
			m_bDragging = true;
			m_vPrevMouse = INPUT->GetMousePos();

			auto actor = OBJECT->GetActor(GUI->GetActorListUI()->GetSelectedActorID());
			if (actor)
			{
				m_vDragStartPos = actor->GetPosition();
			}
		}
	}
}

void Editor::SetupParticleEditorCallback()
{
	GUI->SetParticleEditorCallbck([this](
		shared_ptr<AParticleActor> particleActor,
		const char* texPath,
		const char* shaderPath,
		TransformData actorData,
		Vec2 uvStart,
		Vec2 uvEnd,
		int divisions,
		float duration,
		bool loop,
		bool autoDestroy)
		{
			particleActor->m_szName = L"Particle";

			// 1. 메시 (Quad)
			auto meshComp = UStaticMeshComponent::CreatePlane(); // 또는 CreateQuad()
			particleActor->SetMeshComponent(meshComp);

			// 2. 머티리얼 로드
			auto mat = make_shared<UMaterial>();
			mat->Load(
				std::wstring(texPath, texPath + strlen(texPath)),
				std::wstring(shaderPath, shaderPath + strlen(shaderPath))
			);

			// 3. UV 설정 (셰이더에 넘기기 위함)
			mat->SetUVRange(uvStart, uvEnd); // → SetUVRange() 함수 내부에서 CB 업데이트

			// 4. 머티리얼, 트랜스폼 적용
			meshComp->SetMaterial(mat);

			particleActor->SetMeshComponent(meshComp);
			particleActor->SetPosition(Vec3(actorData.Position));
			particleActor->SetRotation(Vec3(actorData.Rotation));
			particleActor->SetScale(Vec3(actorData.Scale));
			particleActor->InitSpriteAnimation(divisions, duration);
			particleActor->SetLoop(loop);
			particleActor->SetAutoDestroy(autoDestroy);

			// 5. 화면에 등록
			PARTICLE->AddUI(particleActor);
			//particleActor->InitSpriteAnimation(4, 1.f); // 4는 divisions, 10은 frameRate
		});
}

void Editor::Slash()
{
	if (INPUT->GetButtonDown(LCLICK))
	{
		m_bSlashPlaying = true;
		m_fSlashTime = 0.0f;
	}


	if (m_bSlashPlaying)
	{
		m_fSlashTime += TIMER->GetDeltaTime();

		float t = m_fSlashTime;
		float progress = 0.0f;


		if (t <= 0.2f)
		{
			float ratio = t / 0.2f;
			progress = pow(ratio, 2.0f);
		}
		else if (t <= 0.5f)
		{
			progress = 1.0f;
		}
		else
		{
			progress = -1.0f;
		}

		if (m_pSlashMaterial)
			m_pSlashMaterial->SetSlashProgress(progress);

		if (t >= m_fSlashDuration)
		{
			m_bSlashPlaying = false;
		}
	}
}


void Editor::LoadAllPrefabs(const std::string& extension)
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
				shared_ptr<UStaticMeshComponent> meshComp = make_shared<UStaticMeshComponent>();
				if (SplitExt(to_mw(objData.MeshPath)) == L".obj")
				{
					AssimpLoader loader;
					vector<MeshData> meshList = loader.Load(objData.MeshPath.c_str());
					meshComp->SetMeshPath(to_mw(objData.MeshPath)); // 이거 풀네임으로 들어가야되는건가 ? 나중에 어디서 쓰이나 ? 
					auto meshRes = make_shared<UStaticMeshResources>();
					if (!meshList.empty())
					{
						meshRes->SetVertexList(meshList[0].m_vVertexList);
						meshRes->SetIndexList(meshList[0].m_vIndexList);
						meshRes->Create();
						meshComp->SetMesh(meshRes);
					}

				}
				else
				{
					ActorLoader al;
					al.LoadOne(objData.MeshPath);
					meshComp->SetMeshPath(to_mw(objData.MeshPath));
					auto resources = al.LoadMeshResources();
					meshComp->SetMesh(dynamic_pointer_cast<UStaticMeshResources>(resources[0]));
				}


				auto material = make_shared<UMaterial>();
				material->Load(to_mw(objData.TexturePath), to_mw(objData.ShaderPath));
				meshComp->SetMaterial(material);

				auto obj = make_shared<APawn>();
				obj->m_szName = L"Object";
				obj->SetMeshComponent(meshComp);
				obj->SetPosition(objData.Translation);
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
					obj->SetPosition(objData.Translation);
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
