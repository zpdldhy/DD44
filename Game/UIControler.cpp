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
#include "Sound.h"

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

		// ȭ��ǥ Idle
		m_vArrowUI[0]->AddPosition(idle);
		m_vArrowUI[1]->AddPosition(-idle);

		auto temp1 = m_vArrowUI[0]->GetPosition().x;
		auto temp2 = m_vArrowUI[1]->GetPosition().x;

		// Ű���� �Է�
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

		// ���콺 �Է�
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
		// ȭ��ǥ
		m_vArrowUI[0]->AddPosition(Vec3(+TIMER->GetDeltaTime() * 1500.f, 0.f, 0.f));
		m_vArrowUI[1]->AddPosition(Vec3(-TIMER->GetDeltaTime() * 1500.f, 0.f, 0.f));

		// ����â
		for (auto& pUI : m_vMenu)
			pUI->AddPosition(Vec3(0.f, -TIMER->GetDeltaTime() * 1000.f, 0.f));

		// Ÿ��Ʋ
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
	m_pActiveHPTexture = TEXTURE->Get(L"../Resources/Texture/UI/hud_hp_active 1.png");
	m_pEmptyHPTexture = TEXTURE->Get(L"../Resources/Texture/UI/hud_hp_empty 1.png");
	m_pActiveArrowTexture = TEXTURE->Get(L"../Resources/Texture/UI/hud_energy_active.png");
	m_pInActiveArrowTexture = TEXTURE->Get(L"../Resources/Texture/UI/hud_energy_inactive.png");
	m_pEmptyArrowTexture = TEXTURE->Get(L"../Resources/Texture/UI/hud_energy_empty.png");
	m_pUpgradeDoneTexture = TEXTURE->Get(L"../Resources/Texture/UI/upgrade_slot_glow-1.png");

	// InGame UI	
	m_vMainBackGround = PToA->MakeUIs("../Resources/Prefab/UI_Game_BackGround.uis.json");
	m_vHPUI = PToA->MakeUIs("../Resources/Prefab/UI_Game_HP.uis.json");
	m_vArrowUI = PToA->MakeUIs("../Resources/Prefab/UI_Game_Arrow.uis.json");

	UI->AddUIList(m_vMainBackGround);
	UI->AddUIList(m_vHPUI);
	UI->AddUIList(m_vArrowUI);

	// BackGround �и�
	auto iter = m_vHPUI.begin();
	m_pHPBackGround = *iter;
	m_vHPUI.erase(iter);

	// HP �ʱⰪ
	m_vHPUI[4]->m_bRender = false;
	m_vHPUI[5]->m_bRender = false;
	m_vHPUI[6]->m_bRender = false;
	m_vHPUI[7]->m_bRender = false;
	m_vHPUI[8]->m_bRender = false;

	// Arrow �ʱⰪ
	m_vArrowUI[4]->m_bRender = false;
	m_vArrowUI[5]->m_bRender = false;
	m_vArrowUI[6]->m_bRender = false;
	m_vArrowUI[7]->m_bRender = false;
	m_vArrowUI[8]->m_bRender = false;
	
	m_vActiveArrowScale = m_vArrowUI[3]->GetScale();
	m_vInActiveArrowScale = m_vArrowUI[2]->GetScale();

	// Interact UI
	m_vInterActionUI = PToA->MakeUIs("../Resources/Prefab/UI_InterAction.uis.json");
	UI->AddUIList(m_vInterActionUI);
	for (auto& pUI : m_vInterActionUI)
	{
		pUI->m_bRender = false;
	}

	ZeroMemory(&m_tTrigger, sizeof(TriggerData));

	// Paused UI
	m_vPausedBackGround = PToA->MakeUIs("../Resources/Prefab/UI_Paused_BackGround.uis.json");
	UI->AddUIList(m_vPausedBackGround);
	for (auto& pUI : m_vPausedBackGround)
	{
		pUI->m_bRender = false;
	}

	m_vPausedSelect = PToA->MakeUIs("../Resources/Prefab/UI_Paused_Selection.uis.json");
	UI->AddUIList(m_vPausedSelect);
	for (auto& pUI : m_vPausedSelect)
	{
		pUI->m_bRender = false;
	}

	// Paused Upgrade
	m_vUpgradeBackGround = PToA->MakeUIs("../Resources/Prefab/UI_Paused_Upgrade_BackGround.uis.json");
	UI->AddUIList(m_vUpgradeBackGround);
	for (auto& pUI : m_vUpgradeBackGround)
	{
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
			vUI.emplace_back(vUpgradeState[iRow * 8 + iCol]);
		}
		m_vUpgradeState.emplace_back(vUI);
	}

	m_vUpgradeExplain = PToA->MakeUIs("../Resources/Prefab/UI_Paused_Upgrade_Explain.uis.json");
	UI->AddUIList(m_vUpgradeExplain);
	for (auto& pUI : m_vUpgradeExplain)
	{
		pUI->m_bRender = false;
	}

	m_vUpgradeExplain[1]->SetAlignment(DWRITE_TEXT_ALIGNMENT_LEADING, DWRITE_PARAGRAPH_ALIGNMENT_NEAR);

	// Paused System
	m_vSystemBackGround = PToA->MakeUIs("../Resources/Prefab/UI_Paused_System_BackGround.uis.json");
	UI->AddUIList(m_vSystemBackGround);
	for (auto& pUI : m_vSystemBackGround)
	{
		pUI->m_bRender = false;
	}

	m_vSystemSelection = PToA->MakeUIs("../Resources/Prefab/UI_Paused_System_Selection.uis.json");
	UI->AddUIList(m_vSystemSelection);
	for (auto& pUI : m_vSystemSelection)
	{
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
		pUI->m_bRender = false;
	}

	// Sinema
	m_pBettyName = PToA->MakeUI("../Resources/Prefab/UI_Movie_Betty.ui.json");
	UI->AddUI(m_pBettyName);
	m_pBettyName->m_bRun = false;
	m_pBettyName->m_bRender = false;

	m_pEnding = PToA->MakeUI("../Resources/Prefab/UI_Ending.ui.json");
	UI->AddUI(m_pEnding);
	m_pEnding->m_bRun = false;
	m_pEnding->m_bRender = false;
}

void InGameUIControler::Tick()
{
	if (m_bNoRender)	
		return;	

	for (auto& pUI : m_vMainBackGround)
		pUI->m_bRender = true;

	FrameReset();
	UpdateHP();
	UpdateArrow();
	UpdateInteract();
	UpdatePaused();
	UpdateCoin();
	UpdateDead();
	UpdateEnding();
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
	m_pBettyName = nullptr;
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

	// Max HP ����
	static float HPBarSize = m_vHPUI[0]->GetScale().x;
	static Vec3 HPBackSize = m_pHPBackGround->GetScale();
	static Vec3 HPBackPos = m_pHPBackGround->GetPosition();
	
	int n = m_iMaxHP - 4;
	float scale = HPBackSize.x + static_cast<float>(n) * HPBarSize;
	float pos = HPBackPos.x + static_cast<float>(n) * (HPBarSize / 2.f);

	m_pHPBackGround->SetScale(Vec3(scale, HPBackSize.y, HPBackSize.z));
	m_pHPBackGround->SetPosition(Vec3(pos, HPBackPos.y, HPBackPos.z));	

	m_pHPBackGround->m_bRender = true;

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

	// ���� HP�� ���� ����
	for (int i = 0; i < m_iMaxHP; i++)
	{
		m_vHPUI[i]->SetAllTexture(m_pEmptyHPTexture);
		m_vHPUI[i]->SetColor(Color(0.f, 0.f, 0.f, 0.f));
		m_vHPUI[i]->m_bRender = true;
	}

	for (int i = 0; i < m_iCurrentHP; i++)
	{
		if (i > 0)
		{
			m_vHPUI[i - 1]->SetColor(Color(0.055f, 0.247f, -0.324, -0.5f));
		}

		if (i < 9) 
		{
			m_vHPUI[i]->SetAllTexture(m_pActiveHPTexture);
			m_vHPUI[i]->SetColor(Color(0.055f, 0.247f, -0.324, 0.0f));
		}
	}

	// �ǰ� �ٲ�� ����
	//static float currentTime = 0.0f;
	//currentTime = TIMER->GetDeltaTime();

	//if (m_iCurrentHP != m_iPreHP)
	//	m_bHPUIChange = true;

	//if (m_iCurrentHP > 0 &&
	//	m_vHPUI[m_iCurrentHP]->GetColor().w < 0.f &&
	//	m_bHPUIChange)
	//{
	//	m_vHPUI[m_iCurrentHP - 1]->AddColor(Color(0.f, 0.f, 0.f, currentTime / 2));
	//}
	//else if (m_bHPUIChange)
	//{
	//	m_bHPUIChange = false;
	//	currentTime = 0.f;
	//}
	//else if (m_iCurrentHP > 0)
	//	m_vHPUI[m_iCurrentHP - 1]->SetColor(FullHPColor);

	//m_iPreHP = m_iCurrentHP;
}

void InGameUIControler::UpdateArrow()
{
	if (m_iMaxArrow > 9)
		m_iMaxArrow = 9;

	// Max Arrow ����	
	for (int i = 0; i < m_iMaxArrow; i++)
	{
		m_vArrowUI[i]->SetAllTexture(m_pEmptyArrowTexture);
		m_vArrowUI[i]->SetScale(m_vInActiveArrowScale);
		m_vArrowUI[i]->m_bRender = true;
	}

	// Current Arrow ����
	for (int i = 0; i < m_iCurrentArrow; i++)
	{
		if (i > 0)
		{
			m_vArrowUI[i - 1]->SetAllTexture(m_pInActiveArrowTexture);
			m_vArrowUI[i - 1]->SetScale(m_vInActiveArrowScale);
		}

		if (i < 9)
		{
			m_vArrowUI[i]->SetAllTexture(m_pActiveArrowTexture);
			m_vArrowUI[i]->SetScale(m_vActiveArrowScale);
			m_vArrowUI[i]->m_bRender = true;
		}
	}
}

void InGameUIControler::UpdateInteract()
{
	m_bHeal = false;
	// InterAction, ��ȣ�ۿ� UI�� ���� ����
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
				m_vInterActionUI[2]->SetText(L"������");
			}
			else if (m_tTrigger.eTriggerType == ETriggerType::TT_HEALPOINT)
			{
				m_vInterActionUI[2]->SetText(L"ȸ��");
				if(INPUT->GetButton(GameKey::E))
				{
					m_bHeal = true;
				}
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

		// 0�� ���׷��̵�, 1�� ����
		if (INPUT->GetButton(GameKey::A))
			iSelectMenu--;
		else if (INPUT->GetButton(GameKey::D))
			iSelectMenu++;

		if (iSelectMenu < 0)
			iSelectMenu = 0;
		if (iSelectMenu > 1)
			iSelectMenu = 1;

		static int iHealthPoint = 0;
		static int iAttackPoint = 0;
		static int iSpeedPoint = 0;
		static int iArrowPoint = 0;

		switch (iSelectMenu)
		{
			// Upgrade
		case 0:
		{
			iSelectSystem = 0;

			// ���õ� �޴� ���
			m_vPausedSelect[0]->SetColor(Color(0.f, 0.f, 0.f, 0.f));
			m_vPausedSelect[1]->SetColor(Color(0.f, 0.f, 0.f, -0.3f));

			// ���� UI
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

			// ������
			if (INPUT->GetButton(GameKey::W))
				iSelectUpgrade--;
			if (INPUT->GetButton(GameKey::S))
				iSelectUpgrade++;

			if (iSelectUpgrade < 0)
				iSelectUpgrade = 0;
			if (iSelectUpgrade > 3)
				iSelectUpgrade = 3;

			m_iCurrentUpgrade = iSelectUpgrade + 1;

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

			// ���õ� Upgrade�� ���� ����
			m_vUpgradeState[iSelectUpgrade][1]->SetColor(Color(0.f, 0.f, 0.f, 0.f));

			// ���õ� Upgrade�� ������ ���� �κ��� Render�Ѵ�.
			switch (iSelectUpgrade)
			{
			case 0:
			{

				m_vUpgradeExplain[0]->SetText(L"�ִ� ü��");
				m_vUpgradeExplain[1]->SetText(L"�ִ� ü���� �����մϴ�.\n�ִ� ü���� �������� �������� �� ���� ������ �� ������,\n������ ���̳� ������ ���ݵ� ��ƿ �� �ִ� ������ ����ϴ�.");
				if (iHealthPoint < 5)
				{
					m_vUpgradeExplain[5]->m_bRender = false;

					m_vUpgradeExplain[2]->SetText(L"x " + to_wstring(m_iHealthPrice));
				}
				else if (iHealthPoint == 5)	// ��ȭ �Ϸ�
				{
					m_vUpgradeExplain[5]->m_bRender = true;
					m_vUpgradeExplain[2]->m_bRender = false;
					m_vUpgradeExplain[3]->m_bRender = false;
					m_vUpgradeExplain[4]->m_bRender = false;
				}
				break;
			}

			case 1: 
			{
				m_vUpgradeExplain[0]->SetText(L"���� ����");
				m_vUpgradeExplain[1]->SetText(L"���� ���ݷ°� ���� ������ �����մϴ�.\n���� ���� �� ������ �ִ� ���ذ� �����ϸ�,\n������ ������ ���� �� �ִ� ���� �����մϴ�.");
				if (iAttackPoint < 5)
				{
					m_vUpgradeExplain[5]->m_bRender = false;

					m_vUpgradeExplain[2]->SetText(L"x " + to_wstring(m_iAttackPrice));
				}
				else if (iAttackPoint == 5)	// ��ȭ �Ϸ�
				{
					m_vUpgradeExplain[5]->m_bRender = true;
					m_vUpgradeExplain[2]->m_bRender = false;
					m_vUpgradeExplain[3]->m_bRender = false;
					m_vUpgradeExplain[4]->m_bRender = false;
				}
				break;
			}

			case 2:
			{
				m_vUpgradeExplain[0]->SetText(L"�̵� �ӵ�");
				m_vUpgradeExplain[1]->SetText(L"�̵��� �������� �ӵ��� �����մϴ�.\n���� ������ ���ϰų� ������ ��ġ�� �����ϴ� �� ������ �Ǹ�,\n������ �帧�� �����ϰ� �̲� �� �ֽ��ϴ�.");
				if (iSpeedPoint < 5)
				{
					m_vUpgradeExplain[5]->m_bRender = false;

					m_vUpgradeExplain[2]->SetText(L"x " + to_wstring(m_iSpeedPrice));
				}
				else if (iSpeedPoint == 5)	// ��ȭ �Ϸ�
				{
					m_vUpgradeExplain[5]->m_bRender = true;
					m_vUpgradeExplain[2]->m_bRender = false;
					m_vUpgradeExplain[3]->m_bRender = false;
					m_vUpgradeExplain[4]->m_bRender = false;
				}
				break;
			}

			case 3:
			{
				m_vUpgradeExplain[0]->SetText(L"���Ÿ� ����");
				m_vUpgradeExplain[1]->SetText(L"ȭ���� ���ݷ°� ������ �����մϴ�.\n�ָ� �ִ� ���� �� ȿ�������� ó���� �� ������,\n������ �Ÿ����� ������ �̾�� ������ �����մϴ�.");
				if (iArrowPoint < 5)
				{
					m_vUpgradeExplain[5]->m_bRender = false;

					m_vUpgradeExplain[2]->SetText(L"x " + to_wstring(m_iArrowPrice));
				}
				else if (iArrowPoint == 5)	// ��ȭ �Ϸ�
				{
					m_vUpgradeExplain[5]->m_bRender = true;
					m_vUpgradeExplain[2]->m_bRender = false;
					m_vUpgradeExplain[3]->m_bRender = false;
					m_vUpgradeExplain[4]->m_bRender = false;
				}
				break;
			}
			}

			if (INPUT->GetButton(GameKey::SPACE)&& m_iBuyUpgrade == true)
			{
				if (iSelectUpgrade == 0 && iHealthPoint < 5)
				{
					m_vUpgradeState[iSelectUpgrade][iHealthPoint + 2]->SetAllTexture(m_pUpgradeDoneTexture);
					iHealthPoint++;
				}
				else if (iSelectUpgrade == 1 && iAttackPoint < 5)
				{
					m_vUpgradeState[iSelectUpgrade][iAttackPoint + 2]->SetAllTexture(m_pUpgradeDoneTexture);
					iAttackPoint++;
				}
				else if (iSelectUpgrade == 2 && iSpeedPoint < 5)
				{
					m_vUpgradeState[iSelectUpgrade][iSpeedPoint + 2]->SetAllTexture(m_pUpgradeDoneTexture);
					iSpeedPoint++;
				}
				else if (iSelectUpgrade == 3&& iArrowPoint < 5)
				{
					m_vUpgradeState[iSelectUpgrade][iArrowPoint + 2]->SetAllTexture(m_pUpgradeDoneTexture);
					iArrowPoint++;
				}

				SOUND->GetPtr(ESoundType::Allow)->PlayEffect2D();
			}
			// ���ϸ� ��鸮�� ����
			else if (m_iBuyUpgrade == false)
			{
				for (auto& pUI : m_vUpgradeExplain)
				{
					pUI->SetShake(0.5f, 100.f, 5.f, 0.f);
				}
				for (auto& pUI : m_vPausedSelect)
				{
					pUI->SetShake(0.5f, 100.f, 5.f, 0.f);
				}
				for (auto& pUI : m_vUpgradeBackGround)
				{
					pUI->SetShake(0.5f, 100.f, 5.f, 0.f);
				}
				for (auto& vUIList : m_vUpgradeState)
				{
					for (auto& pUI : vUIList)
						pUI->SetShake(0.5f, 100.f, 5.f, 0.f);
				}
				for (auto& pUI : m_vUpgradeExplain)
				{
					pUI->SetShake(0.5f, 100.f, 5.f, 0.f);
				}

				SOUND->GetPtr(ESoundType::Deny)->PlayEffect2D();
			}

			if (iHealthPoint > 5)
				iHealthPoint = 5;
			if (iAttackPoint > 5)
				iAttackPoint = 5;
			if (iSpeedPoint > 5)
				iSpeedPoint = 5;
			if (iArrowPoint > 5)
				iArrowPoint = 5;

			break;
		}
		// System
		case 1:
		{
			iSelectUpgrade = 0;

			// ���õ� �޴� ���
			m_vPausedSelect[0]->SetColor(Color(0.f, 0.f, 0.f, -0.3f));
			m_vPausedSelect[1]->SetColor(Color(0.f, 0.f, 0.f, 0.f));

			// ���� UI
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

			// ������
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
	// Paused �� �ƴ� ��
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
		m_iCurrentUpgrade = 0;
	}
}

void InGameUIControler::UpdateCoin()
{
	wstring szCoin = L"x ";

	for (auto& pUI : m_vCoins)
		pUI->m_bRender = true;

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

		// ��� UI�� �ߴ� �ð�
		if (tempTime > 1.f)
		{
			m_vDeadUI[0]->m_bRun = true;
			m_vDeadUI[0]->m_bRender = true;

			// ������ ȿ��
			float deltaX = tempBackPosition.x + 2.f * cosf(rand());
			float deltaY = tempBackPosition.y + 2.f * cosf(rand());

			m_vDeadUI[0]->SetPosition(Vec3(deltaX, deltaY, 1.f));

			if (tempTime < 4.f) 
			{
				// Ŀ���� ȿ��
				float ratio = tempScale.x / tempScale.y;
				
				// Ŀ�� �ȼ��� 50
				float scaleX = tempScale.x + 50.f * ratio * (tempTime - 1.f) / 3.f;
				float scaleY = tempScale.y + 50.f * (tempTime - 1.f) / 3.f;
				m_vDeadUI[0]->SetScale(Vec3(scaleX, scaleY, 1.f));
			}

			// continue�� �ߴ� �ð�
			if (tempTime > 4.f)
			{
				m_vDeadUI[1]->m_bRun = true;
				m_vDeadUI[2]->m_bRun = true;
				m_vDeadUI[3]->m_bRun = true;
				m_vDeadUI[1]->m_bRender = true;
				m_vDeadUI[2]->m_bRender = true;
				m_vDeadUI[3]->m_bRender = true;

				// ȭ��ǥ �¿� Idle ������
				if (m_bDeadUIMove == false)
				{
					float deltaTime = cosf(3.f * TIMER->GetGameTime()) / 2.f;
					m_vDeadUI[2]->AddPosition(Vec3(-deltaTime, 0.f, 0.f));
					m_vDeadUI[3]->AddPosition(Vec3(deltaTime, 0.f, 0.f));
				}

				// continue���� enter �Ǵ� UI���ý�
				if (INPUT->GetButton(GameKey::SPACE) ||
					m_vDeadUI[1]->GetStateType() == UIStateType::ST_SELECT)
				{
					m_bDeadContinue = true;
					m_bDeadUIMove = true;
				}

				// ȭ��ǥ UI�� ���󰡴� �κ�
				if (m_bDeadUIMove == true)
				{
					float deltaTime = 1000.f * TIMER->GetDeltaTime();
					m_vDeadUI[2]->AddPosition(Vec3(-deltaTime, 0.f, 0.f));
					m_vDeadUI[3]->AddPosition(Vec3(deltaTime, 0.f, 0.f));

					auto toNDC = m_vDeadUI[2]->GetPosition().x / (static_cast<float>(g_windowSize.x) / 2.f);

					// ȭ��ǥ �� ���󰡸� ��
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

			// coin�� ���ڸ� �����ش�
			m_vCoins[1]->m_bRender = false;
			m_vCoins[3]->m_bRender = false;
		}

		SOUND->GetPtr(ESoundType::Stage0)->Stop();
		SOUND->GetPtr(ESoundType::CrowDead)->Play2D(false);
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

void InGameUIControler::UpdateEnding()
{
	if (!m_bGoEnding) return;
	static bool oneFrame = true;
	static wstring ending = L"";

	if (oneFrame)
	{
		auto pos = m_pEnding->GetPosition();
		pos.y = -1500.f;
		m_pEnding->SetPosition(pos);
		m_pEnding->SetAlignment(DWRITE_TEXT_ALIGNMENT_LEADING, DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

		ending += L"����\t\t\t        DD44\n\n";
		ending += L"����\t\t\t        �� ��\n\n";
		ending += L"\t\t\t        �� ����\n\n";
		ending += L"\t\t\t        �� ����\n\n";
		ending += L"\t\t\t        �� ����\n\n";
		ending += L"����\t\t          KGCA ��ī����\n\n\n\n\n\n\n\n\n\n";
		ending += L"       �÷���  ���ּż�  �����մϴ�!";
	}

	m_pEnding->m_bRun = true;
	m_pEnding->m_bRender = true;

	m_pEnding->SetText(ending);

	if (m_pEnding->GetPosition().y < g_windowSize.y*0.74f)
		m_pEnding->AddPosition(Vec3(0.f, TIMER->GetDeltaTime() * 100.f, 0.f));

	oneFrame = false;
}

void InGameUIControler::PopUpBettyName()
{
	m_pBettyName->m_bRun = true;
	m_pBettyName->m_bRender = true;
}

void InGameUIControler::PopDownBettyName()
{
	m_pBettyName->m_bRun = false;
	m_pBettyName->m_bRender = false;
}

void InGameUIControler::NoRenderStateUI()
{
	m_bNoRender = true;

	for (auto& pUI : m_vMainBackGround)
		pUI->m_bRender = false;

	m_pHPBackGround->m_bRender = false;

	for (auto& pUI : m_vHPUI)
		pUI->m_bRender = false;

	for (auto& pUI : m_vArrowUI)
		pUI->m_bRender = false;

	for (auto& pUI : m_vCoins)
		pUI->m_bRender = false;
}
