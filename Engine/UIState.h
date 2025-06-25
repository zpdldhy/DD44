#pragma once

enum class UIStateType
{
	ST_IDLE = 0,
	ST_HOVER,
	ST_ACTIVE,
	ST_SELECT,
	ST_COUNT,
};

enum class UIUseType
{
	UT_MOUSE = 0,
	UT_NOMOUSE,
	UT_COUNT,
};

class AUIActor;

class UIState
{
public:
	UIState(UIUseType _eUseType) : m_eUseType(_eUseType) {}
	virtual ~UIState() = default;
	UIUseType m_eUseType = UIUseType::UT_MOUSE;
	POINT m_ptMousePos = { 0, 0 };

public:
	virtual void ProcessState(AUIActor* _pUI) abstract;
	bool CheckCollision(Vec3 _scale, Vec3 _pos);
};

class IdleUIState : public UIState
{
public:
	IdleUIState(UIUseType _eUseType = UIUseType::UT_MOUSE) : UIState(_eUseType) {}
	virtual void ProcessState(AUIActor* _pUI) override;
};

class HoverUIState : public UIState
{
public:
	HoverUIState(UIUseType _eUseType = UIUseType::UT_MOUSE) : UIState(_eUseType) {}
	virtual void ProcessState(AUIActor* _pUI) override;
};

class ActiveUIState : public UIState
{
public:ActiveUIState(UIUseType _eUseType = UIUseType::UT_MOUSE) : UIState(_eUseType) {}

	virtual void ProcessState(AUIActor* _pUI) override;
};

class SelectUIState : public UIState
{
public:SelectUIState(UIUseType _eUseType = UIUseType::UT_MOUSE) : UIState(_eUseType) {}

	virtual void ProcessState(AUIActor* _pUI) override;
};