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

	// HP
	shared_ptr<class AUIActor> m_pHPBackGround = nullptr;
	vector<shared_ptr<class AUIActor>> m_vHPUI;
	int m_iMaxHP = 4;
	int m_iCurrentHP = 4;
	int m_iPreHP = 0;
	bool m_bHPUIChange = false;

	// Arrow
	int m_iArrowCount = 4;
	vector<shared_ptr<class AUIActor>> m_vArrowUI;
	shared_ptr<class Texture> m_pActiveArrowTexture = nullptr;
	shared_ptr<class Texture> m_pInActiveArrowTexture = nullptr;
	Vec3 m_vActiveArrowScale = { 0.f, 0.f, 0.f };
	Vec3 m_vInActiveArrowScale = { 0.f, 0.f, 0.f };

	// Interact
	vector<shared_ptr<class AUIActor>> m_vInterActionUI;
	struct TriggerData m_tTrigger;

	////////////
	// Paused //
	////////////
	vector<shared_ptr<class AUIActor>> m_vPausedBackGround;
	vector<shared_ptr<class AUIActor>> m_vPausedSelect;

	// Paused Upgrade
	vector<shared_ptr<class AUIActor>> m_vUpgradeBackGround;
	vector<vector<shared_ptr<class AUIActor>>> m_vUpgradeState;
	int m_iHealthPoint = 0;
	int m_iAttackPoint = 0;
	int m_iSpeedPoint = 0;
	int m_iArrowPoint = 0;
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

public:
	void SetMaxHP(int _maxHP) { m_iMaxHP = _maxHP; }
	void SetCurrentHP(int _currentHP) { m_iCurrentHP = _currentHP; }
	void SetArrowCount(int _arrowCount) { m_iArrowCount = _arrowCount; }
	void SetTriggerData(TriggerData _data) { m_tTrigger = _data; }
	void SetUpgradeHealthPoint(int _point) { m_iHealthPoint = _point; }
	void SetUpgradeAttackPoint(int _point) { m_iAttackPoint = _point; }
	void SetUpgradeSpeedPoint(int _point) { m_iSpeedPoint = _point; }
	void SetUpgradeArrowPoint(int _point) { m_iArrowPoint = _point; }
	void SetCoin(int _coin) { m_iCoin = _coin; }
	void SetDead(bool _isDead) { m_bDead = _isDead; }

	bool SelectContinue() { return m_bContinue; }
	bool SelectExit() { return m_bExit; }
	bool SelectDeadContinue() { return m_bDeadContinue; }
	bool EndDeadUI() { return m_bDeadUIEnd; }
};