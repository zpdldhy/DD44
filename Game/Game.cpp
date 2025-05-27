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
#include "AUIActor.h"
#include "UIManager.h"

#include "PrefabToActor.h"

void Game::Init()
{
	Profiler p("Init");
	// Asset ·Îµù

	OBJECT->AddActorList(PToA->LoadAllPrefabs(".map.json"));
	//OBJECT->AddActorList(PToA->LoadAllPrefabs(".object.json"));
	OBJECT->AddActorList(PToA->LoadAllPrefabs(".objects.json"));

	auto vlist = PToA->LoadAllPrefabs(".character.json");
	m_pPlayer = vlist[0];
	OBJECT->AddActorList(vlist);

	UI->AddUIList(PToA->MakeUIs("../Resources/Prefab/UI_Game.uis.json"));

	SetupEngineCamera();
	SetupSkybox();
	SetupSunLight();

}

void Game::Tick()
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
	m_pCameraActor = nullptr;
	m_pPlayer = nullptr;
	m_pSkyMesh = nullptr;
	m_pSky = nullptr;
	m_pSunLight = nullptr;
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
	material->Load(L"../Resources/Texture/Sky.jpg", L"../Resources/Shader/Sky.hlsl");
	m_pSkyMesh->SetMaterial(material);

	OBJECT->AddActor(m_pSky);
}

void Game::SetupSunLight()
{
	m_pSunLight = make_shared<ALight>();
	m_pSunLight->m_szName = L"SunLight";
	m_pSunLight->GetLightComponent()->SetDirection({ 0, -1.f, 0 });
	m_pSunLight->GetLightComponent()->SetAmbientColor(Vec3(1.0f, 1.0f, 1.0f));
	m_pSunLight->GetLightComponent()->SetAmbientPower(0.3f);
	m_pSunLight->SetPosition(Vec3(0, 100.0f, 0));
	m_pSunLight->SetScale(Vec3(10.0f, 10.0f, 10.0f));
	OBJECT->AddActor(m_pSunLight);

	LIGHTMANAGER->Clear();
	LIGHTMANAGER->RegisterLight(m_pSunLight);
}