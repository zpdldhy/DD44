#include "pch.h"
#include "PrefabToActor.h"
#include "PrefabLoader.h"
#include "ActorLoader.h"
#include "MeshLoader.h"
#include "AssimpLoader.h"

// Actor
#include "ATerrainTileActor.h"
#include "APawn.h"
#include "AUIActor.h"

// Component
#include "UStaticMeshComponent.h"
#include "UStaticMeshResources.h"
#include "UBoxComponent.h"

// Script
#include "PlayerMoveScript.h"

unique_ptr<ActorLoader> PrefabToActor::actorLoader = nullptr;
unique_ptr<MeshLoader> PrefabToActor::meshLoader = nullptr;

vector<shared_ptr<class AActor>> PrefabToActor::LoadAllPrefabs(const std::string& extension)
{
	if (!actorLoader || !meshLoader)
		MakeLoader();

	vector<shared_ptr<class AActor>> m_vActorList;

	auto files = PREFAB->GetPrefabFileList("../Resources/Prefab/", extension);

	for (const auto& file : files)
	{
		if (extension == ".map.json")
		{
			m_vActorList.emplace_back(MakeTileActor(file));
		}
		else if (extension == ".character.json")
		{
			m_vActorList.emplace_back(MakeCharacter(file));
		}
		else if (extension == ".object.json")
		{
			m_vActorList.emplace_back(MakeObject(file));
		}
		else if (extension == ".objects.json")
		{
			auto vList = MakeObjects(file);
			m_vActorList.insert(m_vActorList.end(), vList.begin(), vList.end());
		}
		else if (extension == ".ui.json")
		{
			m_vActorList.emplace_back(MakeUI(file));
		}
	}

	return m_vActorList;
}

shared_ptr<ATerrainTileActor> PrefabToActor::MakeTileActor(const string& _file)
{
	shared_ptr<ATerrainTileActor> tile = nullptr;

	PrefabMapData mapData;
	if (PREFAB->LoadMapTile(_file, mapData))
	{
		tile = std::make_shared<ATerrainTileActor>();
		tile->m_szName = L"Terrain";
		tile->m_iNumCols = mapData.Cols;
		tile->m_iNumRows = mapData.Rows;
		tile->m_fCellSize = mapData.CellSize;
		tile->CreateTerrain(to_mw(mapData.TexturePath), to_mw(mapData.ShaderPath));
		tile->SetPosition(mapData.Position);
		tile->SetRotation(mapData.Rotation);
		tile->SetScale(mapData.Scale);
	}

	return tile;
}

shared_ptr<AActor> PrefabToActor::MakeCharacter(const string& _file)
{
	shared_ptr<AActor> actor = nullptr;

	PrefabCharacterData characterData;
	if (PREFAB->LoadCharacter(_file, characterData))
	{
		actor = std::make_shared<AActor>(); // 필요에 따라 캐릭터 타입으로 변경

		shared_ptr<UMeshComponent> meshComponent = meshLoader->Make(characterData.MeshPath.c_str());

		actor->SetMeshComponent(meshComponent);

		actor->m_szName = L"Character";
		actor->SetPosition(Vec3(characterData.transform.Position));
		actor->SetRotation(Vec3(characterData.transform.Rotation));
		actor->SetScale(Vec3(characterData.transform.Scale));

		if (characterData.ScriptType == 1) actor->AddScript(std::make_shared<PlayerMoveScript>());

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
	}

	return actor;
}

shared_ptr<APawn> PrefabToActor::MakeObject(const string& _file)
{
	shared_ptr<APawn> obj = nullptr;

	PrefabObjectData objData;
	if (PREFAB->LoadObject(_file, objData))
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

		obj = make_shared<APawn>();
		obj->m_szName = L"Object";
		obj->SetMeshComponent(meshComp);
		obj->SetPosition(objData.Position);
		obj->SetRotation(objData.Rotation);
		obj->SetScale(objData.Scale);
	}

	return obj;
}

vector<shared_ptr<APawn>> PrefabToActor::MakeObjects(const string& _file)
{
	vector<shared_ptr<APawn>> vObjList;

	std::vector<PrefabObjectData> objList;
	if (PREFAB->LoadObjectArray(_file, objList))
	{
		for (auto& objData : objList)
		{
			auto meshComp = make_shared<UStaticMeshComponent>();
			meshComp->SetMeshPath(to_mw(objData.MeshPath));

			auto resources = actorLoader->LoadOneRes(objData.MeshPath);
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

			vObjList.emplace_back(obj);
		}
	}

	return vObjList;
}

shared_ptr<AUIActor> PrefabToActor::MakeUI(const string& _file)
{
	shared_ptr<AUIActor> uiActor = nullptr;

	PrefabUIData uiData;

	if (PREFAB->LoadUI(_file, uiData))
	{
		uiActor = make_shared<AUIActor>();
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
		uiActor->SetColor(Color(uiData.color));

		uiActor->SetPrefabData(uiData);
	}

	return uiActor;
}

void PrefabToActor::MakeLoader()
{
	actorLoader = make_unique<ActorLoader>();
	meshLoader = make_unique<MeshLoader>();

	actorLoader->LoadAllAsset();
	meshLoader->SetMesh(actorLoader->LoadMeshMap());
	meshLoader->SetAnim(actorLoader->LoadAnimMap());
}
