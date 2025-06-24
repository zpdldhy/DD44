#include "pch.h"
#include "UIControler.h"

// Manager
#include "PrefabToActor.h"
#include "UIManager.h"
#include "ObjectManager.h"
#include "Input.h"
#include "Timer.h"
#include "CameraManager.h"
#include "Engine.h"

// Actor
#include "AUIActor.h"

// Other
#include "Text.h"

void IntroUIControler::init()
{
	m_vBackGround = PToA->MakeUIs("../Resources/Prefab/UI_Intro_BackGround.uis.json");
	m_vMenu = PToA->MakeUIs("../Resources/Prefab/UI_Intro_Menu.uis.json");
	m_vArrowUI = PToA->MakeUIs("../Resources/Prefab/UI_Intro_SelectArrow.uis.json");
	UI->AddUIList(m_vBackGround);
	UI->AddUIList(m_vMenu);
	UI->AddUIList(m_vArrowUI);

	m_vDefaultSelectY = m_vArrowUI[0]->GetPosition().y;
}

void IntroUIControler::Tick()
{
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
			if (m_iHoverMenu != SM_START)
			{
				m_iHoverMenu--;
			}
		}

		if (INPUT->GetButton(DOWN))
		{
			if (m_iHoverMenu != SM_EXIT)
			{
				m_iHoverMenu++;
			}
		}

		// 마우스 입력
		UINT iHover = 0;

		for (auto& menu : m_vMenu)
		{
			if (menu->GetStateType() == UIStateType::ST_HOVER)
				m_iHoverMenu = iHover;
			else if (menu->GetStateType() == UIStateType::ST_SELECT)
			{
				if (iHover == 0)
					m_bSelectStartButton = true;
				else if (iHover == 2)
					m_bSelectEndButton = true;				
			}

			menu->SetColor(Color(0.5f, 0.5f, 0.5f, 1.f));
			iHover++;
		}

		switch (m_iHoverMenu)
		{
		case SM_START:
		{
			m_vMenu[m_iHoverMenu]->SetColor(Color(1.f, 1.f, 1.f, 1.f));

			for (auto& pUI : m_vArrowUI)
			{
				Vec3 NowPos(pUI->GetPosition());
				NowPos.y = m_vDefaultSelectY;
				pUI->SetPosition(NowPos);
			}

			if (INPUT->GetButton(ENTER))
			{
				m_bSelectStartButton = true;
			}
		}
		break;
		case SM_OPTION:
		{
			m_vMenu[m_iHoverMenu]->SetColor(Color(1.f, 1.f, 1.f, 1.f));

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
			m_vMenu[m_iHoverMenu]->SetColor(Color(1.f, 1.f, 1.f, 1.f));

			for (auto& pUI : m_vArrowUI)
			{
				Vec3 NowPos(pUI->GetPosition());
				NowPos.y = m_vDefaultSelectY - 2.f * 0.1333333253860473f * (static_cast<float>(g_windowSize.y) / 2);
				pUI->SetPosition(NowPos);
			}

			if (INPUT->GetButton(ENTER))
			{
				m_bSelectEndButton = true;
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
}

void IntroUIControler::Destroy()
{
	m_vBackGround.clear();
	m_vMenu.clear();
	m_vArrowUI.clear();
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

	// BackGround 분리
	auto iter = m_vHPUI.begin();
	m_pHPBackGround = *iter;
	m_vHPUI.erase(iter);

	// HP 초기값
	m_vHPUI[4]->m_bRun = false;
	m_vHPUI[5]->m_bRun = false;
	m_vHPUI[6]->m_bRun = false;
	m_vHPUI[7]->m_bRun = false;
	m_vHPUI[8]->m_bRun = false;

	m_vHPUI[4]->m_bRender = false;
	m_vHPUI[5]->m_bRender = false;
	m_vHPUI[6]->m_bRender = false;
	m_vHPUI[7]->m_bRender = false;
	m_vHPUI[8]->m_bRender = false;

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

	m_vUpgradeExplain = PToA->MakeUIs("../Resources/Prefab/UI_Paused_Upgrade_Explain.uis.json");
	UI->AddUIList(m_vUpgradeExplain);
	for (auto& pUI : m_vUpgradeExplain)
	{
		pUI->m_bRun = false;
		pUI->m_bRender = false;
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

	m_vSystemSelection[0]->SetState(make_shared<IdleUIState>(UIUseType::UT_NOMOUSE));
	m_vSystemSelection[1]->SetState(make_shared<IdleUIState>(UIUseType::UT_NOMOUSE));

	m_vCoins = PToA->MakeUIs("../Resources/Prefab/UI_Game_Coins.uis.json");
	UI->AddUIList(m_vCoins);

	m_vCoins[1]->SetAlignment(DWRITE_TEXT_ALIGNMENT_LEADING, DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	m_vCoins[3]->SetAlignment(DWRITE_TEXT_ALIGNMENT_LEADING, DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

	// Dead
	m_vDeadUI = PToA->MakeUIs("../Resources/Prefab/UI_Dead.uis.json");
	UI->AddUIList(m_vDeadUI);
	for (auto& pUI : m_vDeadUI)
	{
		pUI->m_bRun = false;
		pUI->m_bRender = false;
	}
}

void InGameUIControler::Tick()
{
	FrameReset();
	UpdateHP();
	UpdateArrow();
	UpdateInteract();
	UpdatePaused();
	UpdateCoin();
	UpdateDead();
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

	m_vDeadUI.clear();
}

void InGameUIControler::FrameReset()
{
	m_bContinue = false;
	m_bExit = false;
	m_bDeadUIEnd = false;
}

void InGameUIControler::UpdateHP()
{
	// HP
	if (m_iMaxHP > 9)
		m_iMaxHP = 9;

	// Max HP 설정
	static float HPBarSize = m_vHPUI[0]->GetScale().x;
	static Vec3 HPBackSize = m_pHPBackGround->GetScale();
	static Vec3 HPBackPos = m_pHPBackGround->GetPosition();
	
	int n = m_iMaxHP - 4;
	float scale = HPBackSize.x + static_cast<float>(n) * HPBarSize;
	float pos = HPBackPos.x + static_cast<float>(n) * (HPBarSize / 2.f);

	m_pHPBackGround->SetScale(Vec3(scale, HPBackSize.y, HPBackSize.z));
	m_pHPBackGround->SetPosition(Vec3(pos, HPBackPos.y, HPBackPos.z));

	// Slice
	if (n == 1)
		m_pHPBackGround->SetSliceData(Vec4(0.125f, 0.125f, 0.5f, 0.5f));
	else if (n == 2)
		m_pHPBackGround->SetSliceData(Vec4(0.1f, 0.1f, 0.5f, 0.5f));
	else if (n == 3)
		m_pHPBackGround->SetSliceData(Vec4(0.1f, 0.1f, 0.5f, 0.5f));
	else if (n == 4)
		m_pHPBackGround->SetSliceData(Vec4(0.075f, 0.075f, 0.5f, 0.5f));
	else if (n == 5)
		m_pHPBackGround->SetSliceData(Vec4(0.075f, 0.075f, 0.5f, 0.5f));

	m_vHPUI[m_iMaxHP - 1]->m_bRun = true;
	m_vHPUI[m_iMaxHP - 1]->m_bRender = true;


	// 현재 HP에 대한 로직
	static Color FullHPColor = Color(0.055f, 0.247f, -0.324, 0.0f);
	static Color RestColor = Color(0.055f, 0.247f, -0.324, -0.5f);

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
		m_vHPUI[3]->SetColor(FullHPColor);
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
			m_vHPUI[2]->SetColor(FullHPColor);
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
			m_vHPUI[1]->SetColor(FullHPColor);
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
			m_vHPUI[0]->SetColor(FullHPColor);

	}
	break;

	case 0:
	{
		m_vHPUI[0]->SetColor(Color(0.f, 0.f, 0.f, -1.f));
	}
	break;
	}

	m_iPreHP = m_iCurrentHP;
}

void InGameUIControler::UpdateArrow()
{
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
}

void InGameUIControler::UpdateInteract()
{
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
}

void InGameUIControler::UpdatePaused()
{
	static int iSelectMenu = 0;
	static int iSelectUpgrade = 0;
	static int iSelectSystem = 0;

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
		if (INPUT->GetButton(GameKey::A))
			iSelectMenu--;
		else if (INPUT->GetButton(GameKey::D))
			iSelectMenu++;

		if (iSelectMenu < 0)
			iSelectMenu = 0;
		if (iSelectMenu > 1)
			iSelectMenu = 1;

		switch (iSelectMenu)
		{
			// Upgrade
		case 0:
		{
			iSelectSystem = 0;

			// 선택된 메뉴 밝게
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
			if (INPUT->GetButton(GameKey::W))
				iSelectUpgrade--;
			if (INPUT->GetButton(GameKey::S))
				iSelectUpgrade++;

			if (iSelectUpgrade < 0)
				iSelectUpgrade = 0;
			if (iSelectUpgrade > 3)
				iSelectUpgrade = 3;

			for (auto& pUI : m_vUpgradeBackGround)
			{
				pUI->m_bRun = true;
				pUI->m_bRender = true;
			}

			for (auto& pUIList : m_vUpgradeState)
			{
				pUIList[1]->SetColor(Color(0.f, 0.f, 0.f, -0.3f));

				for (auto& pUI : pUIList)
				{
					pUI->m_bRun = true;
					pUI->m_bRender = true;
				}
			}

			for (auto& pUI : m_vUpgradeExplain)
			{
				pUI->m_bRun = true;
				pUI->m_bRender = true;
			}

			// 선택된 Upgrade의 색만 변경
			m_vUpgradeState[iSelectUpgrade][1]->SetColor(Color(0.f, 0.f, 0.f, 0.f));

			// 선택된 Upgrade의 설명이 적힌 부분을 Render한다.
			switch (iSelectUpgrade)
			{
			case 0:
			{
				
				break;
			}

			case 1:
				break;

			case 2:
				break;

			case 3:
				break;
			}

			break;
		}
		// System
		case 1:
		{
			iSelectUpgrade = 0;

			// 선택된 메뉴 밝게
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

			for (auto& pUI : m_vUpgradeExplain)
			{
				pUI->m_bRun = false;
				pUI->m_bRender = false;
			}

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

			if (INPUT->GetButton(GameKey::W))
				iSelectSystem--;
			if (INPUT->GetButton(GameKey::S))
				iSelectSystem++;

			if (iSelectSystem < 0)
				iSelectSystem = 0;
			if (iSelectSystem > 1)
				iSelectSystem = 1;

			if (iSelectSystem == 0)
			{
				m_vSystemSelection[0]->SetStateType(UIStateType::ST_HOVER);
				m_vSystemSelection[1]->SetStateType(UIStateType::ST_IDLE);
			}
			else if (iSelectSystem == 1)
			{
				m_vSystemSelection[0]->SetStateType(UIStateType::ST_IDLE);
				m_vSystemSelection[1]->SetStateType(UIStateType::ST_HOVER);
			}

			if (INPUT->CheckKey(GameKey::SPACE))
				m_vSystemSelection[iSelectSystem]->SetStateType(UIStateType::ST_SELECT);

			if (m_vSystemSelection[0]->GetStateType() == UIStateType::ST_SELECT)
				m_bContinue = true;

			if (m_vSystemSelection[1]->GetStateType() == UIStateType::ST_SELECT)
				m_bExit = true;

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

		iSelectUpgrade = 0;
	}
}

void InGameUIControler::UpdateCoin()
{
	wstring szCoin = L"x ";

	m_vCoins[1]->SetText(szCoin + L"0");
	m_vCoins[3]->SetText(szCoin + to_wstring(m_iCoin));
}

void InGameUIControler::UpdateDead()
{
	// Dead
	static float tempTime = 0;
	static Vec3 tempScale = m_vDeadUI[0]->GetScale();
	static Vec3 tempBackPosition = m_vDeadUI[0]->GetPosition();
	static Vec3 tempLPosition = m_vDeadUI[2]->GetPosition();
	static Vec3 tempRPosition = m_vDeadUI[3]->GetPosition();

	if (m_bDead)
	{
		tempTime += TIMER->GetDeltaTime();

		// 사망 UI가 뜨는 시간
		if (tempTime > 1.f)
		{
			m_vDeadUI[0]->m_bRun = true;
			m_vDeadUI[0]->m_bRender = true;

			// 떨리는 효과
			float deltaX = tempBackPosition.x + 2.f * cosf(rand());
			float deltaY = tempBackPosition.y + 2.f * cosf(rand());

			m_vDeadUI[0]->SetPosition(Vec3(deltaX, deltaY, 1.f));

			if (tempTime < 4.f) 
			{
				// 커지는 효과
				float ratio = tempScale.x / tempScale.y;
				
				// 커진 픽셀은 50
				float scaleX = tempScale.x + 50.f * ratio * (tempTime - 1.f) / 3.f;
				float scaleY = tempScale.y + 50.f * (tempTime - 1.f) / 3.f;
				m_vDeadUI[0]->SetScale(Vec3(scaleX, scaleY, 1.f));
			}

			// continue가 뜨는 시간
			if (tempTime > 4.f)
			{
				m_vDeadUI[1]->m_bRun = true;
				m_vDeadUI[2]->m_bRun = true;
				m_vDeadUI[3]->m_bRun = true;
				m_vDeadUI[1]->m_bRender = true;
				m_vDeadUI[2]->m_bRender = true;
				m_vDeadUI[3]->m_bRender = true;

				// 화살표 좌우 Idle 움직임
				if (m_bDeadUIMove == false)
				{
					float deltaTime = cosf(3.f * TIMER->GetGameTime()) / 2.f;
					m_vDeadUI[2]->AddPosition(Vec3(-deltaTime, 0.f, 0.f));
					m_vDeadUI[3]->AddPosition(Vec3(deltaTime, 0.f, 0.f));
				}

				// continue에서 enter 또는 UI선택시
				if (INPUT->GetButton(GameKey::ENTER) ||
					m_vDeadUI[1]->GetStateType() == UIStateType::ST_SELECT)
				{
					m_bDeadContinue = true;
					m_bDeadUIMove = true;
				}

				// 화살표 UI가 날라가는 부분
				if (m_bDeadUIMove == true)
				{
					float deltaTime = 1000.f * TIMER->GetDeltaTime();
					m_vDeadUI[2]->AddPosition(Vec3(-deltaTime, 0.f, 0.f));
					m_vDeadUI[3]->AddPosition(Vec3(deltaTime, 0.f, 0.f));

					auto toNDC = m_vDeadUI[2]->GetPosition().x / (static_cast<float>(g_windowSize.x) / 2.f);

					// 화살표 다 날라가면 끝
					if (toNDC < -1.2f )
					{
						m_bDeadUIMove = false;
						m_bDeadUIEnd = true;
					}
				}
			}

			for (auto& pUI : m_vDeadUI) 
			{
				pUI->m_bRender = true;
			}

			// coin의 글자를 지워준다
			m_vCoins[1]->m_bRender = false;
			m_vCoins[3]->m_bRender = false;
		}
	}
	else
	{
		m_vDeadUI[0]->SetScale(tempScale);
		m_vDeadUI[0]->SetPosition(tempBackPosition);
		m_vDeadUI[2]->SetPosition(tempLPosition);
		m_vDeadUI[3]->SetPosition(tempRPosition);

		for (auto& pUI : m_vDeadUI)
		{
			pUI->m_bRun = false;
			pUI->m_bRender = false;
		}
		tempTime = 0.f;
		m_vCoins[1]->m_bRender = true;
		m_vCoins[3]->m_bRender = true;
	}
}
