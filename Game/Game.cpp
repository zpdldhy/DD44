#include "pch.h"
#include "Game.h"
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
//#include "MeshLoader.h"
#include "Input.h"
#include "UBoxComponent.h"

void Game::Init()
{
	// Asset �ε�
	actorLoader.LoadAllAsset();
	meshLoader.SetMesh(actorLoader.LoadMeshMap());
	meshLoader.SetAnim(actorLoader.LoadAnimMap());

	LoadAllPrefabs(".map.json");
	LoadAllPrefabs(".objects.json");
	LoadAllPrefabs(".object.json");
	LoadAllPrefabs(".character.json");

	SetupEngineCamera();
	SetupSkybox();
	SetupSunLight();

}

void Game::Update()
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

}

void Game::Render()
{
}

void Game::Destroy()
{
}

void Game::SetupEngineCamera()
{
	m_pCameraActor = make_shared<ACameraActor>();

	m_pCameraActor->SetPosition({ 0.0f, 10.0f, 0.0f });
	m_pCameraActor->AddScript(make_shared<EngineCameraMoveScript>());
	m_pCameraActor->m_szName = L"EnginCamera";

	CAMERA->Set3DCameraActor(m_pCameraActor);
	OBJECT->AddActor(m_pCameraActor);
}

void Game::SetupSkybox()
{
	m_pSky = make_shared<ASky>();
	m_pSky->m_szName = L"Sky";
	m_pSkyMesh = UStaticMeshComponent::CreateSphere(20, 20);
	m_pSky->SetMeshComponent(m_pSkyMesh);

	shared_ptr<UMaterial> material = make_shared<UMaterial>();
	material->Load(L"../Resources/Texture/skypano.png", L"../Resources/Shader/Sky.hlsl");
	m_pSkyMesh->SetMaterial(material);

	OBJECT->AddActor(m_pSky);
}

void Game::SetupSunLight()
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

void Game::LoadAllPrefabs(const std::string& extension)
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
				auto actor = std::make_shared<AActor>(); // �ʿ信 ���� ĳ���� Ÿ������ ����

				//ActorLoader actorLoader;
				//actorLoader.LoadAllAsset();
				//MeshLoader meshLoader;
				//meshLoader.SetMesh(actorLoader.LoadMeshMap());
				//meshLoader.SetAnim(actorLoader.LoadAnimMap());

				shared_ptr<UMeshComponent> meshComponent = meshLoader.Make(characterData.MeshPath.c_str());

				actor->SetMeshComponent(meshComponent);

				actor->m_szName = L"Character";
				actor->SetPosition(Vec3(characterData.actor.Position));
				actor->SetRotation(Vec3(characterData.actor.Rotation));
				actor->SetScale(Vec3(characterData.actor.Scale));

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
					if(static_cast<ShapeType>(characterData.shape.eShapeType)== ShapeType::ST_BOX)
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
					meshComp->SetMeshPath(to_mw(objData.MeshPath)); // �̰� Ǯ�������� ���ߵǴ°ǰ� ? ���߿� ��� ���̳� ? 
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
					obj->SetPosition(objData.Translation);
					obj->SetRotation(objData.Rotation);
					obj->SetScale(objData.Scale);

					OBJECT->AddActor(obj);
				}
			}
		}

	}


}
