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
	m_vBackGround = PToA->MakeUIs("../Resources/Prefab/UI_Intro_BackGround.uis.json");
	m_vMenu = PToA->MakeUIs("../Resources/Prefab/UI_Intro_Menu.uis.json");
	m_vArrowUI = PToA->MakeUIs("../Resources/Prefab/UI_Intro_SelectArrow.uis.json");
	UI->AddUIList(m_vBackGround);
	UI->AddUIList(m_vMenu);
	UI->AddUIList(m_vArrowUI);	

	m_vDefaultSelectY = m_vArrowUI[0]->GetPosition().y;

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
	m_vMenu.clear();
	m_vArrowUI.clear();
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
	if (m_iPrevMenu != m_vSelectMenu)
	{
		m_iPrevMenu = m_vSelectMenu;
		SOUND->GetPtr(ESoundType::Hover)->PlayEffect2D();
	}

	if (!m_bSelectStartButton || !m_bSelectEndButton)
	{
		Vec3 idle(cosf(TIMER->GetGameTime() * 7.f) * 0.2f, 0.f, 0.f);

		// 화살표 Idle
		m_vArrowUI[0]->AddPosition(idle);
		m_vArrowUI[1]->AddPosition(-idle);

		auto temp1 = m_vArrowUI[0]->GetPosition().x;
		auto temp2 = m_vArrowUI[1]->GetPosition().x;

		// 키보드 입력
		if (INPUT->GetButton(UP))
		{
			if (m_vSelectMenu != SM_START)
			{
				m_vSelectMenu--;
			}
		}

		if (INPUT->GetButton(DOWN))
		{
			if (m_vSelectMenu != SM_EXIT)
			{
				m_vSelectMenu++;
			}
		}

		// 마우스 입력
		UINT iSelect = 0;

		for (auto& menu : m_vMenu)
		{
			if (menu->GetStateType() == UIStateType::ST_HOVER)
				m_vSelectMenu = iSelect;
			else if (menu->GetStateType() == UIStateType::ST_SELECT)
			{
				if (iSelect == 0)
					m_bSelectStartButton = true;
				else if (iSelect == 2)
					m_bSelectEndButton = true;

				SOUND->GetPtr(ESoundType::Click)->PlayEffect2D();
			}		

			menu->SetColor(Color(0.5f, 0.5f, 0.5f, 1.f));
			iSelect++;
		}

		switch (m_vSelectMenu)
		{
		case SM_START:
		{
			m_vMenu[m_vSelectMenu]->SetColor(Color(1.f, 1.f, 1.f, 1.f));

			for (auto& pUI : m_vArrowUI)
			{
				Vec3 NowPos(pUI->GetPosition());
				NowPos.y = m_vDefaultSelectY;
				pUI->SetPosition(NowPos);
			}

			if (INPUT->GetButton(ENTER))
			{
				m_bSelectStartButton = true;
				SOUND->GetPtr(ESoundType::Click)->PlayEffect2D(); // 추가
			}
		}
		break;
		case SM_OPTION:
		{
			m_vMenu[m_vSelectMenu]->SetColor(Color(1.f, 1.f, 1.f, 1.f));

			for (auto& pUI : m_vArrowUI)
			{
				Vec3 NowPos(pUI->GetPosition());
				NowPos.y = m_vDefaultSelectY - 0.1333333253860473f * (static_cast<float>(g_windowSize.y) / 2);
				pUI->SetPosition(NowPos);
			}

		}
		break;
		case SM_EXIT:
		{
			m_vMenu[m_vSelectMenu]->SetColor(Color(1.f, 1.f, 1.f, 1.f));

			for (auto& pUI : m_vArrowUI)
			{
				Vec3 NowPos(pUI->GetPosition());
				NowPos.y = m_vDefaultSelectY - 2.f * 0.1333333253860473f * (static_cast<float>(g_windowSize.y) / 2);
				pUI->SetPosition(NowPos);
			}

			if (INPUT->GetButton(ENTER))
			{
				m_bSelectEndButton = true;
				SOUND->GetPtr(ESoundType::Click)->PlayEffect2D(); // 추가
			}
		}
		break;
		}
	}

	// UIThrow
	if (m_bSelectStartButton)
	{
		// 화살표
		m_vArrowUI[0]->AddPosition(Vec3(+TIMER->GetDeltaTime() * 1500.f, 0.f, 0.f));
		m_vArrowUI[1]->AddPosition(Vec3(-TIMER->GetDeltaTime() * 1500.f, 0.f, 0.f));

		// 선택창
		for (auto& pUI : m_vMenu)
			pUI->AddPosition(Vec3(0.f, -TIMER->GetDeltaTime() * 1000.f, 0.f));

		// 타이틀
		m_vBackGround[1]->AddPosition(Vec3(0.f, TIMER->GetDeltaTime() * 1000.f, 0.f));

		auto what = m_vMenu[0]->GetPosition().y;

		if ((m_vMenu[0]->GetPosition().y + (m_vMenu[0]->GetScale().y / 2) < -1.f * (static_cast<float>(g_windowSize.y) / 2.f)) &&
			(m_vBackGround[1]->GetPosition().y - (m_vBackGround[1]->GetScale().y / 2) > 1.f * (static_cast<float>(g_windowSize.y) / 2.f)))
		{
			m_bthrowUI = true;
			m_bSelectStartButton = false;
		}
	}

	if (m_bSelectEndButton)
		PostQuitMessage(0);

	// Do FadeIn
	if (m_bthrowUI)
	{
		UI->DoFadeIn();
		m_bthrowUI = false;
	}

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
