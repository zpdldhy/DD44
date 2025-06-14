#include "pch.h"
#include "PrefabToActor.h"
#include "PrefabLoader.h"
#include "ActorLoader.h"
#include "MeshLoader.h"
#include "AssimpLoader.h"
#include "ParticleManager.h"

// Actor
#include "ATerrainTileActor.h"
#include "APawn.h"
#include "AUIActor.h"
#include "AParticleActor.h"
#include "AFireParticleActor.h"
// Game Actor
#include "TPlayer.h"
#include "TEnemy.h"
#include "TInteractable.h"

// Component
#include "UStaticMeshComponent.h"
#include "UStaticMeshResources.h"
#include "UBoxComponent.h"
#include "USphereComponent.h"

//// Script
#include "ScriptManager.h"

//unique_ptr<ActorLoader> PrefabToActor::actorLoader = nullptr;
//unique_ptr<MeshLoader> PrefabToActor::meshLoader = nullptr;

void PrefabToActor::Init()
{
	MakeLoader();
}

vector<shared_ptr<class AActor>> PrefabToActor::LoadAllPrefabs(const std::string& extension)
{
	//MakeLoader();

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
		else if (extension == ".uis.json")
		{
			auto vList = MakeUIs(file);
			m_vActorList.insert(m_vActorList.end(), vList.begin(), vList.end());
		}
		else if (extension == ".particlegroup.json")
		{
			auto particles = PrefabToActor::MakeParticleGroup(file);
			PARTICLE->AddParticleList(particles);
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
		tile->m_eActorType = ActorType::AT_GROUND;
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
	shared_ptr<TCharacter> actor = nullptr;

	PrefabCharacterData characterData;
	if (PREFAB->LoadCharacter(_file, characterData))
	{
		if (characterData.ScriptName == "PlayerMoveScript")
		{
			actor = std::make_shared<TPlayer>();
		}
		else if (characterData.m_eActorType == 7)
		{
			actor = std::make_shared<TInteractable>();
		}
		else
		{
			actor = std::make_shared<TEnemy>();
		}

		shared_ptr<UMeshComponent> meshComponent = MESHLOADER->Make(characterData.MeshPath.c_str());

		actor->SetMeshComponent(meshComponent);

		actor->m_szName = to_mw(characterData.Name);
		actor->m_eActorType = static_cast<ActorType>(characterData.m_eActorType);
		actor->SetPosition(Vec3(characterData.transform.Position));
		actor->SetRotation(Vec3(characterData.transform.Rotation));
		actor->SetScale(Vec3(characterData.transform.Scale));

		auto script = SCRIPT->GetScript(to_mw(characterData.ScriptName));
		actor->AddScript(script);

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
			else if (static_cast<ShapeType>(characterData.shape.eShapeType) == ShapeType::ST_SPHERE)
				shapeComponent = make_shared<USphereComponent>();

			shapeComponent->SetLocalScale(Vec3(characterData.shape.Scale));
			shapeComponent->SetLocalPosition(Vec3(characterData.shape.Position));
			shapeComponent->SetLocalRotation(Vec3(characterData.shape.Rotation));
			shapeComponent->SetCollisionEnabled(CollisionEnabled::CE_QUERYANDPHYSICS);
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
		shared_ptr<UStaticMeshComponent> meshComp = make_shared<UStaticMeshComponent>();
		if (SplitExt(to_mw(objData.MeshPath)) == L".obj")
		{
			//Profiler p("Mesh From Obj");
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
			//Profiler p("Mesh From Asset");
			auto resources = ACTORLOADER->LoadOneRes(objData.MeshPath);
			meshComp->SetMesh(dynamic_pointer_cast<UStaticMeshResources>(resources));
			meshComp->SetMeshPath(to_mw(objData.MeshPath));
		}

		auto material = make_shared<UMaterial>();
		material->Load(to_mw(objData.TexturePath), to_mw(objData.ShaderPath));
		meshComp->SetMaterial(material);

		obj = make_shared<APawn>();
		obj->SetPrefabPath(_file);
		obj->m_szName = L"Object";
		obj->SetMeshComponent(meshComp);
		obj->SetPosition(objData.Position);
		obj->SetRotation(objData.Rotation);
		obj->SetScale(objData.Scale);

		if (objData.ShapeData.isUse)
		{
			shared_ptr<UShapeComponent> shapeComp = nullptr;

			if (objData.ShapeData.eShapeType == ShapeType::ST_BOX)
				shapeComp = std::make_shared<UBoxComponent>();
			else if (objData.ShapeData.eShapeType == ShapeType::ST_SPHERE)
				shapeComp = std::make_shared<USphereComponent>();
			// else if (...) // 다른 타입 추가 가능

			shapeComp->SetLocalScale(Vec3(objData.ShapeData.Scale));
			shapeComp->SetLocalPosition(Vec3(objData.ShapeData.Position));
			shapeComp->SetLocalRotation(Vec3(objData.ShapeData.Rotation));
			shapeComp->SetCollisionEnabled(CollisionEnabled::CE_QUERYANDPHYSICS);

			obj->SetShapeComponent(shapeComp);
		}
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

			auto resources = ACTORLOADER->LoadOneRes(objData.MeshPath);
			meshComp->SetMesh(dynamic_pointer_cast<UStaticMeshResources>(resources));

			auto material = make_shared<UMaterial>();
			material->Load(to_mw(objData.TexturePath), to_mw(objData.ShaderPath));
			meshComp->SetMaterial(material);

			auto obj = make_shared<APawn>();
			obj->SetPrefabPath(_file);
			obj->m_szName = L"Object";
			obj->SetMeshComponent(meshComp);
			obj->SetPosition(objData.Position);
			obj->SetRotation(objData.Rotation);
			obj->SetScale(objData.Scale);

			if (objData.ShapeData.isUse)
			{
				shared_ptr<UShapeComponent> shapeComp = nullptr;

				if (objData.ShapeData.eShapeType == ShapeType::ST_BOX)
					shapeComp = std::make_shared<UBoxComponent>();

				shapeComp->SetLocalScale(Vec3(objData.ShapeData.Scale));
				shapeComp->SetLocalPosition(Vec3(objData.ShapeData.Position));
				shapeComp->SetLocalRotation(Vec3(objData.ShapeData.Rotation));
				shapeComp->SetCollisionEnabled(CollisionEnabled::CE_QUERYANDPHYSICS);

				obj->SetShapeComponent(shapeComp);
			}

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
		uiActor->m_bTextUI = uiData.isTextUI;

		auto meshComp = UStaticMeshComponent::CreatePlane();
		uiActor->SetMeshComponent(meshComp);

		auto mat = make_shared<UMaterial>();
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
		uiActor->SetColor(Color(uiData.color));
		uiActor->SetFontSize(uiData.FontSize);
		uiActor->SetFontPath(to_mw(uiData.FontPath));
		uiActor->SetText(to_mw(uiData.Text));
		uiActor->SetSliceData(Vec4(uiData.SliceUV));

		uiActor->SetPrefabData(uiData);
	}

	return uiActor;
}

vector<shared_ptr<AUIActor>> PrefabToActor::MakeUIs(const string& _file)
{
	vector<shared_ptr<AUIActor>> vList;

	vector<PrefabUIData> uiDatas;
	if (PREFAB->LoadUIs(_file, uiDatas))
	{
		for (auto& uiData : uiDatas)
		{
			auto uiActor = make_shared<AUIActor>();
			uiActor->m_szName = to_mw(uiData.Name);
			uiActor->m_bTextUI = uiData.isTextUI;

			auto meshComp = UStaticMeshComponent::CreatePlane();
			uiActor->SetMeshComponent(meshComp);

			auto mat = make_shared<UMaterial>();
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
			uiActor->SetColor(Color(uiData.color));
			uiActor->SetFontSize(uiData.FontSize);
			uiActor->SetFontPath(to_mw(uiData.FontPath));
			uiActor->SetText(to_mw(uiData.Text));
			uiActor->SetSliceData(Vec4(uiData.SliceUV));

			uiActor->SetPrefabData(uiData);

			vList.emplace_back(uiActor);
		}
	}

	return vList;
}

vector<shared_ptr<class AParticleActor>> PrefabToActor::MakeParticleGroup(const std::string& _filePath)
{
	vector<shared_ptr<AParticleActor>> result;

	PrefabParticleGroupData group;
	if (PREFAB->LoadParticleGroup(group, _filePath))
	{
		for (auto& p : group.Particles)
		{
			shared_ptr<AParticleActor> newParticle;

			if (p.Type == EParticleType::Fire)
			{
				auto fire = make_shared<AFireParticleActor>();
				fire->SetAmplitude(p.Amplitude);
				fire->SetRandomFreq(p.RandomFreq);
				fire->SetRandomOffset(p.RandomOffset);
				fire->SetTimeOffset(p.TimeOffset);
				newParticle = fire;
			}
			else
			{
				newParticle = make_shared<AParticleActor>();
			}

			// 트랜스폼
			newParticle->SetPosition(p.Translation);
			newParticle->SetRotation(p.Rotation);
			newParticle->SetScale(p.Scale);

			// 메시 및 머티리얼
			auto mesh = UStaticMeshComponent::CreatePlane();
			auto mat = make_shared<UMaterial>();
			mat->Load(to_mw(p.TexturePath), to_mw(p.ShaderPath));
			mesh->SetMaterial(mat);

			newParticle->SetMeshComponent(mesh);

			// 애니메이션 세팅
			newParticle->SetUV(p.UVStart, p.UVEnd);
			newParticle->InitSpriteAnimation(p.Divisions, p.Duration);
			newParticle->SetLoop(p.bLoop);
			newParticle->SetAutoDestroy(p.bAutoDestroy);

			result.push_back(newParticle);
		}
	}

	return result;
}

void PrefabToActor::MakeLoader()
{
	//actorLoader = make_unique<ActorLoader>();
	//meshLoader = make_unique<MeshLoader>();

	//actorLoader->LoadAllAsset();
	//meshLoader->SetMesh(actorLoader->LoadMeshMap());
	//meshLoader->SetAnim(actorLoader->LoadAnimMap());
	ACTORLOADER->LoadAllAsset();
	MESHLOADER->SetMesh(ACTORLOADER->LoadMeshMap());
	MESHLOADER->SetAnim(ACTORLOADER->LoadAnimMap());


}
