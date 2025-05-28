#include "pch.h"
#include "IntroScene.h"
#include "CameraManager.h"
#include "LightManager.h"
#include "ObjectManager.h"
#include "UIManager.h"
#include "PrefabToActor.h"

// Actor
#include "ACameraActor.h"
#include "ASky.h"
#include "ALight.h"

// component
#include "UMaterial.h"
#include "UStaticMeshComponent.h"
#include "ULightComponent.h"

// Script
#include "EngineCameraMoveScript.h"

IntroScene::~IntroScene()
{
	OBJECT->RemoveAll();
}

void IntroScene::Init()
{
	UI->AddUIList(PToA->MakeUIs("../Resources/Prefab/UI_IntroScene.uis.json"));

	SetupEngineCamera();
	//SetupSkybox();
	SetupSunLight();
}

void IntroScene::Tick()
{
}

void IntroScene::Render()
{
}

void IntroScene::Destroy()
{
}

void IntroScene::SetupEngineCamera()
{
	auto pCameraActor = make_shared<ACameraActor>();

	pCameraActor->SetPosition({ 0.0f, 10.0f, 0.0f });
	pCameraActor->AddScript(make_shared<EngineCameraMoveScript>());
	pCameraActor->m_szName = L"EnginCamera";

	CAMERA->Set3DCameraActor(pCameraActor);
	OBJECT->AddActor(pCameraActor);
}

void IntroScene::SetupSkybox()
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

void IntroScene::SetupSunLight()
{
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