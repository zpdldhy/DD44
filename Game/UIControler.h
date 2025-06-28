#pragma once
#include "TriggerData.h"

class UIControler
{
public:
	virtual void init() abstract;
	virtual void Tick() abstract;
	virtual void Destroy() abstract;
};

enum SelectMenu
{
	SM_START = 0,
	SM_OPTION,
	SM_EXIT,
	SM_COUNT,
};

class IntroUIControler : public UIControler
{
	vector<shared_ptr<class AUIActor>> m_vBackGround;
	vector<shared_ptr<class AUIActor>> m_vMenu;
	vector<shared_ptr<class AUIActor>> m_vArrowUI;
	UINT m_iHoverMenu = 0;
	float m_vDefaultSelectY = 0.f;
	bool m_bSelectStartButton = false;
	bool m_bSelectEndButton = false;
	bool m_bthrowUI = false;

public:
	void init() override;
	void Tick() override;
	void Destroy() override;

public:
	int GetPrevMenu() { return m_iHoverMenu; }
	bool IsSelectMenu() { return m_bSelectStartButton || m_bSelectEndButton; }
};

class InGameUIControler : public UIControler
{
	////////////
	// InGame //
	////////////
	vector<shared_ptr<class AUIActor>> m_vMainBackGround;
	bool m_bNoRender = false;

	// HP
	shared_ptr<class AUIActor> m_pHPBackGround = nullptr;
	shared_ptr<class Texture> m_pActiveHPTexture = nullptr;
	shared_ptr<class Texture> m_pEmptyHPTexture = nullptr;
	vector<shared_ptr<class AUIActor>> m_vHPUI;
	int m_iMaxHP = 4;
	int m_iCurrentHP = 4;
	int m_iPreHP = 0;
	bool m_bHPUIChange = false;

	// Arrow
	vector<shared_ptr<class AUIActor>> m_vArrowUI;
	shared_ptr<class Texture> m_pActiveArrowTexture = nullptr;
	shared_ptr<class Texture> m_pInActiveArrowTexture = nullptr;
	shared_ptr<class Texture> m_pEmptyArrowTexture = nullptr;
	int m_iMaxArrow = 4;
	int m_iCurrentArrow = 4;
	Vec3 m_vActiveArrowScale = { 0.f, 0.f, 0.f };
	Vec3 m_vInActiveArrowScale = { 0.f, 0.f, 0.f };

	// Interact
	vector<shared_ptr<class AUIActor>> m_vInterActionUI;
	struct TriggerData m_tTrigger;
	bool m_bHeal = false;

	////////////
	// Paused //
	////////////
	vector<shared_ptr<class AUIActor>> m_vPausedBackGround;
	vector<shared_ptr<class AUIActor>> m_vPausedSelect;

	// Paused Upgrade
	vector<shared_ptr<class AUIActor>> m_vUpgradeBackGround;
	vector<vector<shared_ptr<class AUIActor>>> m_vUpgradeState;
	shared_ptr<class Texture> m_pUpgradeDoneTexture = nullptr;
	int m_iCurrentUpgrade = 0;
	int m_iHealthPrice = 100;
	int m_iAttackPrice = 100;
	int m_iSpeedPrice = 100;
	int m_iArrowPrice = 100;
	bool m_iBuyUpgrade = true;

	vector<shared_ptr<class AUIActor>> m_vUpgradeExplain;

	// Paused System
	vector<shared_ptr<class AUIActor>> m_vSystemBackGround;
	vector<shared_ptr<class AUIActor>> m_vSystemSelection;

	bool m_bContinue = false;
	bool m_bExit = false;

	////////////
	//  Coin  //
	////////////
	vector<shared_ptr<class AUIActor>> m_vCoins;
	int m_iCoin = 0;

	////////////
	//  Dead  //
	////////////
	vector<shared_ptr<class AUIActor>> m_vDeadUI;
	bool m_bDead = false;
	bool m_bDeadContinue = false;
	bool m_bDeadUIMove = false;
	bool m_bDeadUIEnd = false;

	//////////////
	//  Sinema  //
	//////////////
	shared_ptr<class AUIActor> m_pBettyName = nullptr;
	vector<shared_ptr<class AUIActor>> m_vEndingImage;
	vector<shared_ptr<class AUIActor>> m_vEndingText;
	shared_ptr<class AUIActor> m_pEndingFade = nullptr;
	bool m_bGoEnding = false;

public:
	void init() override;
	void Tick() override;
	void Destroy() override;

	void FrameReset();

private:
	void UpdateHP();
	void UpdateArrow();
	void UpdateInteract();
	void UpdatePaused();
	void UpdateCoin();
	void UpdateDead();
	void UpdateEnding();

public:
	void SetMaxHP(int _maxHP) { m_iMaxHP = _maxHP; }
	void SetCurrentHP(int _currentHP) { m_iCurrentHP = _currentHP; }
	void SetMaxArrow(int _maxArrow) { m_iMaxArrow = _maxArrow; }
	void SetCurrentArrow(int _arrowCount) { m_iCurrentArrow = _arrowCount; }
	void SetTriggerData(TriggerData _data) { m_tTrigger = _data; }
	void SetCoin(int _coin) { m_iCoin = _coin; }
	void SetHealthPrice(int _price) { m_iHealthPrice = _price; }
	void SetAttackPrice(int _price) { m_iAttackPrice = _price; }
	void SetSpeedPrice(int _price) { m_iSpeedPrice = _price; }
	void SetArrowPrice(int _price) { m_iArrowPrice = _price; }
	void IsBuyUpgrade(bool _isBuy) { m_iBuyUpgrade = _isBuy; }
	void SetDead(bool _isDead) { m_bDead = _isDead; }

	int  CurrentUpgrade() { return m_iCurrentUpgrade; }
	bool SelectContinue() { return m_bContinue; }
	bool SelectExit() { return m_bExit; }
	bool SelectDeadContinue() { return m_bDeadContinue; }
	bool EndDeadUI() { return m_bDeadUIEnd; }
	bool IsHealAction() { return m_bHeal; }

	void PopUpBettyName();
	void PopDownBettyName();

	void NoRenderStateUI();
	void RenderStateUI() { m_bNoRender = false; }

	void GoEnding() { m_bGoEnding = true; }
};