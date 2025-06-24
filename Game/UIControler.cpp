#include "pch.h"
#include "UIControler.h"

// Manager
#include "PrefabToActor.h"
#include "UIManager.h"
#include "ObjectManager.h"
#include "Timer.h"
#include "CameraManager.h"
#include "Engine.h"

// Actor
#include "AUIActor.h"


void IntroUIControler::init()
{
}

void IntroUIControler::Tick()
{
}

void IntroUIControler::Destroy()
{
}

///////////////////////////////////////////////////////////////////////////////
///////////						IN GAME								///////////
///////////////////////////////////////////////////////////////////////////////

void InGameUIControler::init()
{
	// InGame UI	
	m_vMainBackGround = PToA->MakeUIs("../Resources/Prefab/UI_Game_BackGround.uis.json");
	m_vHPUI = PToA->MakeUIs("../Resources/Prefab/UI_Game_HP.uis.json");
	m_vArrowUI = PToA->MakeUIs("../Resources/Prefab/UI_Game_Arrow.uis.json");

	UI->AddUIList(m_vMainBackGround);
	UI->AddUIList(m_vHPUI);
	UI->AddUIList(m_vArrowUI);

	m_pActiveArrowTexture = TEXTURE->Get(L"Resources/Texture/UI/hud_energy_active.png");
	m_pInActiveArrowTexture = TEXTURE->Get(L"Resources/Texture/UI/hud_energy_inactive.png");

	m_vActiveArrowScale = m_vArrowUI[3]->GetScale();
	m_vInActiveArrowScale = m_vArrowUI[2]->GetScale();

	// Interact UI
	m_vInterActionUI = PToA->MakeUIs("../Resources/Prefab/UI_InterAction.uis.json");
	UI->AddUIList(m_vInterActionUI);
	for (auto& pUI : m_vInterActionUI)
	{
		pUI->m_bRender = false;
		pUI->m_bRun = false;
	}

	ZeroMemory(&m_tTrigger, sizeof(TriggerData));

	// Paused UI
	m_vPausedBackGround = PToA->MakeUIs("../Resources/Prefab/UI_Paused_BackGround.uis.json");
	UI->AddUIList(m_vPausedBackGround);
	for (auto& pUI : m_vPausedBackGround)
	{
		pUI->m_bRun = false;
		pUI->m_bRender = false;
	}

	m_vPausedSelect = PToA->MakeUIs("../Resources/Prefab/UI_Paused_Selection.uis.json");
	UI->AddUIList(m_vPausedSelect);
	for (auto& pUI : m_vPausedSelect)
	{
		pUI->m_bRun = false;
		pUI->m_bRender = false;
	}

	// Paused Upgrade
	m_vUpgradeBackGround = PToA->MakeUIs("../Resources/Prefab/UI_Paused_Upgrade_BackGround.uis.json");
	UI->AddUIList(m_vUpgradeBackGround);
	for (auto& pUI : m_vUpgradeBackGround)
	{
		pUI->m_bRun = false;
		pUI->m_bRender = false;
	}

	auto vUpgradeState = PToA->MakeUIs("../Resources/Prefab/UI_Paused_Upgrade_State.uis.json");
	UI->AddUIList(vUpgradeState);
	for (int iRow = 0; iRow < 4; iRow++)
	{
		vector<shared_ptr<AUIActor>> vUI;
		for (int iCol = 0; iCol < 8; iCol++)
		{
			vUpgradeState[iRow * 8 + iCol]->m_bRender = false;
			vUpgradeState[iRow * 8 + iCol]->m_bRun = false;
			vUI.emplace_back(vUpgradeState[iRow * 8 + iCol]);
		}
		m_vUpgradeState.emplace_back(vUI);
	}

	// Paused System
	m_vSystemBackGround = PToA->MakeUIs("../Resources/Prefab/UI_Paused_System_BackGround.uis.json");
	UI->AddUIList(m_vSystemBackGround);
	for (auto& pUI : m_vSystemBackGround)
	{
		pUI->m_bRun = false;
		pUI->m_bRender = false;
	}

	m_vSystemSelection = PToA->MakeUIs("../Resources/Prefab/UI_Paused_System_Selection.uis.json");
	UI->AddUIList(m_vSystemSelection);
	for (auto& pUI : m_vSystemSelection)
	{
		pUI->m_bRun = false;
		pUI->m_bRender = false;
	}

	m_vCoins = PToA->MakeUIs("../Resources/Prefab/UI_Game_Coins.uis.json");
	UI->AddUIList(m_vCoins);

	// Dead
	m_pDeadUI = PToA->MakeUI("../Resources/Prefab/UI_Dead.ui.json");
	UI->AddUI(m_pDeadUI);
}

void InGameUIControler::Tick()
{
	// HP
	Color RestColor;
	RestColor = fullHP;
	RestColor.w = -0.5f;

	static float currentTime = 0.0f;
	currentTime = TIMER->GetDeltaTime();

	if (m_iCurrentHP != m_iPreHP)
		m_bHPUIChange = true;

	switch (m_iCurrentHP)
	{
	case 4:
	{
		m_vHPUI[0]->SetColor(RestColor);
		m_vHPUI[1]->SetColor(RestColor);
		m_vHPUI[2]->SetColor(RestColor);
		m_vHPUI[3]->SetColor(fullHP);
	}
	break;

	case 3:
	{
		if (m_vHPUI[2]->GetColor().w < 0.f && m_bHPUIChange)
		{
			m_vHPUI[3]->SetColor(Color(0.f, 0.f, 0.f, -1.f));
			m_vHPUI[2]->AddColor(Color(0.f, 0.f, 0.f, currentTime / 2));
		}
		else if (m_bHPUIChange == true)
			m_bHPUIChange = false;
		else
		{
			m_vHPUI[0]->SetColor(RestColor);
			m_vHPUI[1]->SetColor(RestColor);
			m_vHPUI[2]->SetColor(fullHP);
		}
	}
	break;

	case 2:
	{
		if (m_vHPUI[1]->GetColor().w < 0.f && m_bHPUIChange)
		{
			m_vHPUI[2]->SetColor(Color(0.f, 0.f, 0.f, -1.f));
			m_vHPUI[1]->AddColor(Color(0.f, 0.f, 0.f, currentTime / 2));
		}
		else if (m_bHPUIChange == true)
			m_bHPUIChange = false;
		else
		{
			m_vHPUI[0]->SetColor(RestColor);
			m_vHPUI[1]->SetColor(fullHP);
		}
	}
	break;

	case 1:
	{
		if (m_vHPUI[0]->GetColor().w < 0.f && m_bHPUIChange)
		{
			m_vHPUI[1]->SetColor(Color(0.f, 0.f, 0.f, -1.f));
			m_vHPUI[0]->AddColor(Color(0.f, 0.f, 0.f, currentTime / 2));
		}
		else if (m_bHPUIChange == true)
			m_bHPUIChange = false;
		else
			m_vHPUI[0]->SetColor(fullHP);

	}
	break;

	case 0:
	{
		m_vHPUI[0]->SetColor(Color(0.f, 0.f, 0.f, -1.f));
	}
	break;
	}

	m_iPreHP = m_iCurrentHP;

	// Arrow
	switch (m_iArrowCount)
	{
	case 4:
		m_vArrowUI[0]->SetAllTexture(m_pInActiveArrowTexture);
		m_vArrowUI[1]->SetAllTexture(m_pInActiveArrowTexture);
		m_vArrowUI[2]->SetAllTexture(m_pInActiveArrowTexture);
		m_vArrowUI[3]->SetAllTexture(m_pActiveArrowTexture);

		m_vArrowUI[0]->SetScale(m_vInActiveArrowScale);
		m_vArrowUI[1]->SetScale(m_vInActiveArrowScale);
		m_vArrowUI[2]->SetScale(m_vInActiveArrowScale);
		m_vArrowUI[3]->SetScale(m_vActiveArrowScale);

		m_vArrowUI[0]->m_bRender = true;
		m_vArrowUI[1]->m_bRender = true;
		m_vArrowUI[2]->m_bRender = true;
		m_vArrowUI[3]->m_bRender = true;
		break;

	case 3:
		m_vArrowUI[0]->SetAllTexture(m_pInActiveArrowTexture);
		m_vArrowUI[1]->SetAllTexture(m_pInActiveArrowTexture);
		m_vArrowUI[2]->SetAllTexture(m_pActiveArrowTexture);

		m_vArrowUI[0]->SetScale(m_vInActiveArrowScale);
		m_vArrowUI[1]->SetScale(m_vInActiveArrowScale);
		m_vArrowUI[2]->SetScale(m_vActiveArrowScale);

		m_vArrowUI[0]->m_bRender = true;
		m_vArrowUI[1]->m_bRender = true;
		m_vArrowUI[2]->m_bRender = true;
		m_vArrowUI[3]->m_bRender = false;
		break;

	case 2:
		m_vArrowUI[0]->SetAllTexture(m_pInActiveArrowTexture);
		m_vArrowUI[1]->SetAllTexture(m_pActiveArrowTexture);

		m_vArrowUI[0]->SetScale(m_vInActiveArrowScale);
		m_vArrowUI[1]->SetScale(m_vActiveArrowScale);

		m_vArrowUI[0]->m_bRender = true;
		m_vArrowUI[1]->m_bRender = true;
		m_vArrowUI[2]->m_bRender = false;
		m_vArrowUI[3]->m_bRender = false;
		break;

	case 1:
		m_vArrowUI[0]->SetAllTexture(m_pActiveArrowTexture);

		m_vArrowUI[0]->SetScale(m_vActiveArrowScale);

		m_vArrowUI[0]->m_bRender = true;
		m_vArrowUI[1]->m_bRender = false;
		m_vArrowUI[2]->m_bRender = false;
		m_vArrowUI[3]->m_bRender = false;
		break;

	case 0:
		m_vArrowUI[0]->m_bRender = false;
		m_vArrowUI[1]->m_bRender = false;
		m_vArrowUI[2]->m_bRender = false;
		m_vArrowUI[3]->m_bRender = false;
		break;
	}

	// InterAction, 상호작용 UI를 띄우는 구간
	//auto vTriggerList = dynamic_pointer_cast<TPlayer>(m_pPlayer)->GetTrigger();
	if (m_tTrigger.eTriggerType != ETriggerType::TT_NONE)
	{
		static Vec3 offsetPos1 = m_vInterActionUI[1]->GetPosition();
		static Vec3 offsetPos2 = m_vInterActionUI[2]->GetPosition();

		auto pos = CAMERA->GetScreenPos(m_tTrigger.vPoint);

		m_vInterActionUI[0]->SetPosition(pos);
		m_vInterActionUI[1]->SetPosition(pos + offsetPos1);
		m_vInterActionUI[2]->SetPosition(pos + offsetPos2);

		for (auto& pUI : m_vInterActionUI)
		{
			pUI->m_bRun = true;
			pUI->m_bRender = true;

			if (m_tTrigger.eTriggerType == ETriggerType::TT_LADDER)
			{

			}
			else if (m_tTrigger.eTriggerType == ETriggerType::TT_HEALPOINT)
			{

			}
		}
	}
	else
	{
		for (auto& pUI : m_vInterActionUI)
		{
			pUI->m_bRun = false;
			pUI->m_bRender = false;
		}
	}

	// Paused
	if (ENGINE->m_bGamePaused == true)
	{
		for (auto& pUI : m_vPausedBackGround)
		{
			pUI->m_bRun = true;
			pUI->m_bRender = true;
		}

		for (auto& pUI : m_vPausedSelect)
		{
			pUI->m_bRun = true;
			pUI->m_bRender = true;
		}

		// 0이 없그레이드, 1이 설정
		if (m_vPausedSelect[0]->GetStateType() == UIStateType::ST_SELECT)
			m_iSelectUI = 0;
		else if (m_vPausedSelect[1]->GetStateType() == UIStateType::ST_SELECT)
			m_iSelectUI = 1;

		switch (m_iSelectUI)
		{
			// Upgrade
		case 0:
		{
			// 선택된 옵션 밝게
			m_vPausedSelect[0]->SetColor(Color(0.f, 0.f, 0.f, 0.f));
			m_vPausedSelect[1]->SetColor(Color(0.f, 0.f, 0.f, -0.3f));

			// 꺼줄 UI
			for (auto& pUI : m_vSystemBackGround)
			{
				pUI->m_bRun = false;
				pUI->m_bRender = false;
			}

			for (auto& pUI : m_vSystemSelection)
			{
				pUI->m_bRun = false;
				pUI->m_bRender = false;
			}

			// 설정값
			for (auto& pUI : m_vUpgradeBackGround)
			{
				pUI->m_bRun = true;
				pUI->m_bRender = true;
			}

			UINT iSelect = 1;
			for (auto& pUIList : m_vUpgradeState)
			{
				// 해당 Upgrade 부분을 선택하면 색깔이 바뀌게 하는 로직
				if (pUIList[0]->GetStateType() == UIStateType::ST_SELECT)
				{
					m_iSelectUpgradeUI = iSelect;
				}

				if (iSelect == m_iSelectUpgradeUI)
					pUIList[1]->SetColor(Color(0.f, 0.f, 0.f, 0.0f));
				else
					pUIList[1]->SetColor(Color(0.f, 0.f, 0.f, -0.3f));

				for (auto& pUI : pUIList)
				{
					pUI->m_bRun = true;
					pUI->m_bRender = true;
				}

				iSelect++;
			}

			// 선택된 Upgrade의 설명이 적힌 부분을 Render한다.
			switch (m_iSelectUpgradeUI)
			{
			case 1:
				break;

			case 2:
				break;

			case 3:
				break;

			case 4:
				break;
			default:
				break;
			}

			break;
		}
		// System
		case 1:
		{
			// 선택된 옵션 밝게
			m_vPausedSelect[0]->SetColor(Color(0.f, 0.f, 0.f, -0.3f));
			m_vPausedSelect[1]->SetColor(Color(0.f, 0.f, 0.f, 0.f));

			// 꺼줄 UI
			for (auto& pUI : m_vUpgradeBackGround)
			{
				pUI->m_bRun = false;
				pUI->m_bRender = false;
			}

			for (auto& pUIList : m_vUpgradeState)
				for (auto& pUI : pUIList)
				{
					pUI->m_bRun = false;
					pUI->m_bRender = false;
				}

			m_iSelectUpgradeUI = 0;

			// 설정값
			for (auto& pUI : m_vSystemBackGround)
			{
				pUI->m_bRun = true;
				pUI->m_bRender = true;
			}

			for (auto& pUI : m_vSystemSelection)
			{
				pUI->m_bRun = true;
				pUI->m_bRender = true;
			}

			if (m_vSystemSelection[0]->GetStateType() == UIStateType::ST_SELECT)
				ENGINE->m_bGamePaused = false;

			if (m_vSystemSelection[1]->GetStateType() == UIStateType::ST_SELECT)
				ENGINE->m_bRun = false;

			break;
		}
		}

	}
	// Paused 가 아닐 때
	else
	{
		for (auto& pUI : m_vPausedBackGround)
		{
			pUI->m_bRun = false;
			pUI->m_bRender = false;
		}

		for (auto& pUI : m_vPausedSelect)
		{
			pUI->m_bRun = false;
			pUI->m_bRender = false;
		}

		for (auto& pUI : m_vUpgradeBackGround)
		{
			pUI->m_bRun = false;
			pUI->m_bRender = false;
		}

		for (auto& pUIList : m_vUpgradeState)
			for (auto& pUI : pUIList)
			{
				pUI->m_bRun = false;
				pUI->m_bRender = false;
			}

		for (auto& pUI : m_vSystemBackGround)
		{
			pUI->m_bRun = false;
			pUI->m_bRender = false;
		}

		for (auto& pUI : m_vSystemSelection)
		{
			pUI->m_bRun = false;
			pUI->m_bRender = false;
		}

		m_iSelectUpgradeUI = 0;
	}

	// End
	static float tempTime = 0;
	if (m_bDead)
	{
		tempTime += TIMER->GetDeltaTime();

		if (tempTime > m_fDeadUIPopTime)
		{
			m_pDeadUI->m_bRender = true;
			m_vCoins[1]->m_bRender = false;
			m_vCoins[3]->m_bRender = false;
		}
	}
	else
	{
		m_pDeadUI->m_bRender = false;
		tempTime = 0.f;
		m_vCoins[1]->m_bRender = true;
		m_vCoins[3]->m_bRender = true;
	}
}

void InGameUIControler::Destroy()
{
	m_vMainBackGround.clear();
	m_vHPUI.clear();
	m_vArrowUI.clear();
	m_vInterActionUI.clear();

	m_vPausedBackGround.clear();
	m_vPausedSelect.clear();
	m_vUpgradeBackGround.clear();
	m_vUpgradeState.clear();
	m_vCoins.clear();

	m_vSystemBackGround.clear();
	m_vSystemSelection.clear();

	m_pDeadUI = nullptr;
}
