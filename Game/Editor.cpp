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
#include "PrefabToActor.h"
#include "EffectManager.h"

bool bRunGame = true;

//#undef RUN_GAME
void Editor::Init()
{	
	// Asset 로딩
	actorLoader.LoadAllAsset();
	meshLoader.SetMesh(actorLoader.LoadMeshMap());
	meshLoader.SetAnim(actorLoader.LoadAnimMap());

	//actorLoader.ConvertFbxToAsset("../Resources/Obj/*.fbx");
	//objectLoader.ConvertObjToAsset("../Resources/Obj/*.obj");
	
	{
		m_pSlashMaterial = make_shared<UMaterial>();
		m_pSlashMaterial->Load(
			L"../Resources/Texture/sword_slash_texture1.png",
			L"../Resources/Shader/Slash.hlsl"
		);
	}
	
	SetupEditorCallbacks();

	OBJECT->AddActorList(PToA->LoadAllPrefabs(".map.json"));
	//OBJECT->AddActorList(PToA->LoadAllPrefabs(".object.json"));
	OBJECT->AddActorList(PToA->LoadAllPrefabs(".objects.json"));	

	auto vlist = PToA->LoadAllPrefabs(".character.json");
	m_pPlayer = vlist[0];
	OBJECT->AddActorList(vlist);

	SetupEngineCamera();
	SetupSkybox();
	SetupSunLight();	
}

void Editor::Tick()
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

	if (INPUT->GetButton(J))
	{
		Vec3 pos = Vec3(0.0f, 1.0f, 0.0f);
		Vec3 velocity = Vec3(0, 10, -10); // 위로 튀게
		for (int i = 0; i < 5; ++i)
		{
			EFFECT->PlayEffect(EEffectType::Blood, pos, 45.0f, velocity);
		}
	}

	if (GUI->GetObjectEditorUI()->IsPlacementMode() && INPUT->GetButton(LCLICK))
	{
		CreateObjectAtMousePick();
	}

	TransformActorByDragging();
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

	float _boxScale = 135.0f;
	float _boxPosition = 100.0f;

	m_pGizmoX = make_shared<AActor>();
	m_pGizmoX->m_szName = L"Gizmo";
	m_pGizmoX->SetMeshComponent(meshComponent->GetChild(0));
	shared_ptr<UBoxComponent> _pXBox = std::make_shared<UBoxComponent>();
	_pXBox->m_bVisible = false;
	_pXBox->SetShapeColor(Vec4(1.0f, 0.f, 0.f, 0.f));
	_pXBox->SetLocalScale(Vec3(_boxScale, 10.0f, 10.0f));
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
	_pYBox->SetLocalScale(Vec3(10.0f, _boxScale, 10.0f));
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
	_pZBox->SetLocalScale(Vec3(10.0f, 10.0f, _boxScale));
	_pZBox->SetLocalPosition(Vec3(0.0f, 0.0f, _boxPosition));
	_pZBox->SetCollisionEnabled(CollisionEnabled::CE_QUERYONLY);
	m_pGizmoZ->SetShapeComponent(_pZBox);
	m_pGizmoZ->SetScale(m_pGizmoCore->GetScale());
	m_pGizmoZ->SetPosition(m_pGizmoCore->GetPosition());
	OBJECT->AddActor(m_pGizmoZ);
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
			auto actor = make_shared<APawn>();
			actor->m_szName = L"Object";

			shared_ptr<UStaticMeshComponent> meshComp = make_shared<UStaticMeshComponent>();
			if (SplitExt(to_mw(objPath)) == L".obj")
			{
				//Profiler p("Mesh From Obj");
				AssimpLoader loader;
				vector<MeshData> meshList = loader.Load(objPath);
				meshComp->SetMeshPath(to_mw(objPath)); // 이거 풀네임으로 들어가야되는건가 ? 나중에 어디서 쓰이나 ? 
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
				//Profiler p("Mesh From Asset");
				auto resources = actorLoader.LoadOneRes(objPath);
				meshComp->SetMesh(dynamic_pointer_cast<UStaticMeshResources>(resources));
				meshComp->SetMeshPath(to_mw(objPath));
			}

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

			auto mat = make_shared<UMaterial>();
			mat->Load(to_mw(texPath), to_mw(shaderPath));
			meshComp->SetMaterial(mat);

			// Snap 적용 여부 확인
			if (GUI->GetObjectEditorUI()->IsSnapEnabled())
			{
				pos = GUI->GetObjectEditorUI()->SnapToGrid(pos, 10.0f);
			}

			actor->SetMeshComponent(meshComp);
			actor->SetPosition(pos);
			actor->SetRotation(rot);
			actor->SetScale(scale);

			OBJECT->AddActor(actor);
		});
}

void Editor::SetupUIEditorCallback()
{
	GUI->SetUIEditorCallback([this](shared_ptr<AUIActor> uiActor, const char* texPath, const char* shaderPath, TransformData actorData, Color _color, Vec4 sliceUV)
		{
			uiActor->m_szName = L"UI";
			auto meshComp = UStaticMeshComponent::CreatePlane();
			uiActor->SetMeshComponent(meshComp);

			auto mat = make_shared<UMaterial>();
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
			uiActor->SetColor(_color);

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
		if (!pActor || pActor->m_szName == L"Terrain") return;

		if (pActor->m_szName != L"Gizmo")
		{
			auto& vec = GUI->GetActorListUI()->GetMultiSelectedIDs();
			if (INPUT->GetButtonDown(LSHIFT)/* && INPUT->GetButton(LCLICK)*/)
			{
				GUI->GetActorListUI()->SetMultiSelectMode(true);

				auto found = std::find(vec.begin(), vec.end(), pActor->m_Index);
				if (found != vec.end())
					vec.erase(found);
				else
					vec.push_back(pActor->m_Index);

				GUI->GetActorListUI()->SetSelectedActorID(pActor->m_Index);
			}
			else
			{
				GUI->GetActorListUI()->SetMultiSelectMode(false);

				vec.clear();
				vec.push_back(pActor->m_Index);
				GUI->GetActorListUI()->SetSelectedActorID(pActor->m_Index);
			}

			GUI->GetActorListUI()->SetSelectedGizmoAxis(-1);
			GUI->GetActorListUI()->RefreshSelectionVisuals();
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
				m_vDragStartRot = actor->GetRotation();
				m_vDragStartScale = actor->GetScale();
			}
		}
	}
}

void Editor::SetGizmoPosition(Vec3 _pos)
{
	float offset = 10.0f;

	m_pGizmoCore->SetPosition(Vec3(_pos.x + offset, _pos.y + 1.0f, _pos.z - offset));
	m_pGizmoX->SetPosition(m_pGizmoCore->GetPosition());
	m_pGizmoY->SetPosition(m_pGizmoCore->GetPosition());
	m_pGizmoZ->SetPosition(m_pGizmoCore->GetPosition());
}

void Editor::TransformActorByDragging()
{
	if (GUI->GetObjectEditorUI()->IsPlacementMode()) return;

	if (INPUT->GetButton(LCLICK))
	{
		SetClickPos();
	}

	if (INPUT->GetButtonDown(LCLICK))
	{
		UINT selectedID = GUI->GetActorListUI()->GetSelectedActorID();
		if (selectedID > 0 && OBJECT->GetActorList().count(selectedID) > 0)
		{
			if (!m_pGizmoCore)
			{
				SetupGizmo();
			}

			auto& actorMap = OBJECT->GetActorList();
			auto _actor = actorMap.find(selectedID);
			if (_actor != actorMap.end())
			{
				auto actor = _actor->second;
				if (actor)
				{
					SetGizmoPosition(actor->GetPosition());
				}
				else
				{
					SetGizmoPosition(Vec3(0.f, 0.f, 0.f));
				}
			}

			auto _axis = actorMap.find(GUI->GetActorListUI()->GetSelectedGizmoAxis());
			if (_axis != actorMap.end())
			{
				auto actor = _actor->second;
				auto axis = _axis->second;
				if (!actor || !axis) return;

				POINT curMouse = INPUT->GetMousePos();
				float deltaX = static_cast<float>(curMouse.x - m_vPrevMouse.x);
				float deltaY = static_cast<float>(curMouse.y - m_vPrevMouse.y);
				Vec3 newPos = m_vDragStartPos;
				Vec3 newRot = m_vDragStartRot;
				Vec3 newScale = m_vDragStartScale;

				auto mode = GUI->GetActorListUI()->GetMode();

				switch (mode)
				{
				case GizmoMode::Translate:
				{
					if (axis == m_pGizmoX)
						newPos.x += deltaX * 0.03f;
					else if (axis == m_pGizmoY)
						newPos.y -= deltaY * 0.03f;
					else if (axis == m_pGizmoZ)
						newPos.z += deltaX * 0.03f;

					Vec3 offset = newPos - m_vDragStartPos;

					// 모든 선택된 애들에게 동일 offset 적용
					const auto& selectedIDs = GUI->GetActorListUI()->GetMultiSelectedIDs();
					for (UINT id : selectedIDs)
					{
						auto a = OBJECT->GetActor(id);
						if (!a || a == actor) continue; // 기준 Actor 제외
						a->SetPosition(a->GetPosition() + offset);
					}

					actor->SetPosition(actor->GetPosition() + offset);
					SetGizmoPosition(actor->GetPosition());
					break;
				}
				case GizmoMode::Rotate:
				{
					if (axis == m_pGizmoX)
						newRot.x += deltaX * 0.03f;
					else if (axis == m_pGizmoY)
						newRot.y -= deltaY * 0.03f;
					else if (axis == m_pGizmoZ)
						newRot.z += deltaX * 0.03f;

					Vec3 offset = newRot - m_vDragStartRot;

					const auto& selectedIDs = GUI->GetActorListUI()->GetMultiSelectedIDs();
					for (UINT id : selectedIDs)
					{
						auto a = OBJECT->GetActor(id);
						if (!a || a == actor) continue;
						a->SetRotation(a->GetRotation() + offset);
					}

					actor->SetRotation(actor->GetRotation() + offset);
					break;
				}
				case GizmoMode::Scale:
				{
					if (axis == m_pGizmoX)
						newScale.x += deltaX * 0.03f;
					else if (axis == m_pGizmoY)
						newScale.y -= deltaY * 0.03f;
					else if (axis == m_pGizmoZ)
						newScale.z += deltaX * 0.03f;

					Vec3 offset = newScale - m_vDragStartScale;

					const auto& selectedIDs = GUI->GetActorListUI()->GetMultiSelectedIDs();
					for (UINT id : selectedIDs)
					{
						auto a = OBJECT->GetActor(id);
						if (!a || a == actor) continue;
						a->SetScale(a->GetScale() + offset);
					}

					actor->SetScale(actor->GetScale() + offset);
					break;
				}
				}

				m_vPrevMouse = curMouse;
			}
		}
	}
	else if (INPUT->GetButtonUp(LCLICK))
	{
		m_bDragging = false;
		GUI->GetActorListUI()->SetSelectedGizmoAxis(-1);
	}
}

void Editor::CreateObjectAtMousePick()
{
	MouseRay ray;
	ray.Click();

	shared_ptr<AActor> tileActor;
	if (!Collision::CheckRayCollision(ray, OBJECT->GetActorIndexList(), tileActor))
		return;

	// 진짜 Terrain이랑 충돌한 경우에만 처리
	if (!tileActor || tileActor->m_szName != L"Terrain")
		return;

	auto terrain = std::dynamic_pointer_cast<ATerrainTileActor>(tileActor);
	if (!terrain || !terrain->GetShapeComponent())
		return;

	Box box = static_pointer_cast<UBoxComponent>(terrain->GetShapeComponent())->GetBounds();
	Vec3 hitPos;
	Ray castRay(ray.position, ray.direction);

	if (!Collision::CheckAABBToRay(castRay, box, hitPos))
		return;

	float x = hitPos.x;
	float z = hitPos.z;
	float y = terrain->GetHeightAt(x, z);

	Vec3 placePos(x, y, z);
	if (GUI->GetObjectEditorUI()->IsSnapEnabled())
		placePos = GUI->GetObjectEditorUI()->SnapToGrid(placePos, 10.0f);

	GUI->GetObjectEditorUI()->CreateAtPosition(placePos);
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


		if (t <= 0.4f)
		{
			float ratio = t / 0.4f;
			progress = pow(ratio, 2.0f);
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