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
	vector<shared_ptr<class AUIActor>> m_vHPUI;
	Color fullHP = { 0.055f, 0.247f, -0.324, 0.0f };
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
	UINT m_iSelectUI = 0;
	vector<shared_ptr<class AUIActor>> m_vPausedBackGround;
	vector<shared_ptr<class AUIActor>> m_vPausedSelect;

	// Paused Upgrade
	UINT m_iSelectUpgradeUI = 0;	// 0이 None, 1 ~ 4 적용
	vector<shared_ptr<class AUIActor>> m_vUpgradeBackGround;
	vector<vector<shared_ptr<class AUIActor>>> m_vUpgradeState;
	vector<shared_ptr<class AUIActor>> m_vCoins;

	// Paused System
	vector<shared_ptr<class AUIActor>> m_vSystemBackGround;
	vector<shared_ptr<class AUIActor>> m_vSystemSelection;

	////////////
	//  Dead  //
	////////////
	shared_ptr<class AUIActor> m_pDeadUI = nullptr;
	bool m_bDead = false;
	float m_fDeadUIPopTime = 1.f;

public:
	void init() override;
	void Tick() override;
	void Destroy() override;

public:
	// 구현 필요!
	void SetMaxHP(int _maxHP) {}
	void SetCurrentHP(int _currentHP) { m_iCurrentHP = _currentHP; }
	void SetArrowCount(int _arrowCount) { m_iArrowCount = _arrowCount; }
	void SetTriggerData(TriggerData _data) { m_tTrigger = _data; }
	void SetDead(bool _isDead) { m_bDead = _isDead; }
};