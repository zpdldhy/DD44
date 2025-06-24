#include "pch.h"
#include "IntroScene.h"

// Manager
#include "CameraManager.h"
#include "LightManager.h"
#include "ObjectManager.h"
#include "UIManager.h"
#include "Input.h"
#include "PrefabToActor.h"
#include "Engine.h"
#include "Timer.h"

// Actor
#include "ACameraActor.h"
#include "ASky.h"
#include "ALight.h"
#include "AUIActor.h"

// component
#include "UMaterial.h"
#include "UStaticMeshComponent.h"
#include "ULightComponent.h"

// Script
#include "EngineCameraMoveScript.h"

// Sound
#include "Sound.h"

// Scene
#include "Game.h"

void IntroScene::Init()
{
	UI->SetFadeActor(PToA->MakeUI("../Resources/Prefab/UI_Fade.ui.json"));
	m_cUI.init();

	SetupEngineCamera();
}

void IntroScene::Tick()
{
	SOUND->GetPtr(ESoundType::Intro)->Play2D();
	UpdateUIState();
}

void IntroScene::Render()
{
}

void IntroScene::Destroy()
{
	m_cUI.Destroy();
	OBJECT->RemoveAll();
	UI->RemoveAll();
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

void IntroScene::UpdateUIState()
{
	if (m_iPrevMenu != m_cUI.GetPrevMenu())
	{
		m_iPrevMenu = m_cUI.GetPrevMenu();
		SOUND->GetPtr(ESoundType::Hover)->PlayEffect2D();
	}

	m_cUI.Tick();

	if(m_cUI.IsSelectMenu())
		SOUND->GetPtr(ESoundType::Click)->PlayEffect2D();

	// Select Start Button
	if (UI->GetFadeWorkDone())
	{
		Destroy();
		auto game = make_shared<Game>();
		game->Init();
		Engine::GetInstance()->SetApp(game);
		SOUND->GetPtr(ESoundType::Intro)->Stop();
	}
}
